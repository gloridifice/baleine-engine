use std::{
    collections::{BTreeMap, HashMap, HashSet},
    sync::{Arc, LazyLock, Mutex},
};

use crate::{
    c_parser::{CEnum, CStruct},
    generator::{vk_enum::VkEnumGenerator, vk_struct::VkStructGenerator},
    str_utils::camel_to_snake,
};

pub mod vk_enum;
pub mod vk_struct;

const SUFFIX: [&'static str; 5] = ["", "2", "KHR", "EXT", "NV"];

pub static VK_ENUM_SET: LazyLock<Arc<Mutex<HashSet<String>>>> =
    LazyLock::new(|| Arc::new(Mutex::new(Default::default())));

pub static PRIMITIVE_MAP: LazyLock<HashMap<&'static str, &'static str>> = LazyLock::new(|| {
    [
        ("uint32_t", "u32"),
        ("uint16_t", "u16"),
        ("int32_t", "i32"),
        ("int16_t", "i16"),
        ("int", "i32"),
        ("unsigned int", "u32"),
        ("float", "f32"),
        ("double", "f64"),
        ("bool", "bool"),
        ("VkBool32", "bool"),
    ]
    .into_iter()
    .collect()
});

const VK_ENUM_BLACKLIST: LazyLock<HashSet<&'static str>> =
    LazyLock::new(|| ["VkStructureType"].into_iter().collect());

const VK_STRUCT_BLACKLIST: LazyLock<HashSet<&'static str>> =
    LazyLock::new(|| [].into_iter().collect());

pub trait ElementsGenerator<T> {
    fn element(&mut self, element: &T, context: &Context) -> Option<String>;
    fn generate(
        &mut self,
        elements: impl Iterator<Item = impl AsRef<T>>,
        context: &Context,
    ) -> String {
        let mut ret = String::new();
        for ele in elements {
            if let Some(ele) = self.element(ele.as_ref(), context) {
                ret.push_str(&ele);
                ret.push('\n');
            }
        }
        ret
    }
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum VkStructType {
    Normal,
    SType,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum VkEnumType {
    Normal,
    Flags,
}

#[derive(Clone)]
pub struct VkStruct {
    pub c_struct: CStruct,
    pub ty: VkStructType,
    pub new_name: String,
}

#[derive(Clone)]
pub struct VkEnum {
    c_enum: CEnum,
    ty: VkEnumType,
    new_name: String,
}

pub struct Context {
    vk_enums: BTreeMap<String, Arc<VkEnum>>,
    vk_structs: BTreeMap<String, Arc<VkStruct>>,
}

pub struct Generator {
    context: Context,
}

impl Generator {
    pub fn new(c_enums: Vec<CEnum>, c_structs: Vec<CStruct>) -> Self {
        let c_enums = c_enums
            .into_iter()
            .filter(|it| it.name.starts_with("Vk") && !VK_ENUM_BLACKLIST.contains(it.name.as_str()))
            .collect();

        let c_structs = c_structs
            .into_iter()
            .filter(|it| {
                it.name.starts_with("Vk") && !VK_STRUCT_BLACKLIST.contains(it.name.as_str())
            })
            .collect();

        Self {
            context: Context::new(c_enums, c_structs),
        }
    }

    pub fn generate(&self) -> String {
        let mut vk_struct_generator = VkStructGenerator;
        let mut vk_enum_generator = VkEnumGenerator;
        let Self {
            context: Context {
                vk_enums,
                vk_structs,
            },
        } = self;

        [
            vk_enum_generator.generate(vk_enums.values(), &self.context),
            vk_struct_generator.generate(vk_structs.values(), &self.context),
        ]
        .join("\n")
    }
}

impl VkEnum {
    const FLAGS_SUFFIX: LazyLock<Vec<String>> = LazyLock::new(|| {
        let mut ret = Vec::new();
        for main in ["Flags", "FlagBits"] {
            for sufix in &SUFFIX {
                let mut end = main.to_string();
                end.push_str(sufix);
                ret.push(end);
            }
        }
        ret
    });

    pub fn new(c_enum: CEnum) -> Self {
        let ty = Self::parse_type(&c_enum.name);
        let new_name = Self::parse_new_name(&c_enum.name, ty);

        Self {
            c_enum,
            ty,
            new_name,
        }
    }

    fn parse_type(name: &str) -> VkEnumType {
        if Self::FLAGS_SUFFIX.iter().any(|it| name.ends_with(it)) {
            VkEnumType::Flags
        } else {
            VkEnumType::Normal
        }
    }

    fn parse_new_name(name: &str, ty: VkEnumType) -> String {
        let ret = name.replace("Vk", "");
        match ty {
            VkEnumType::Normal => ret,
            VkEnumType::Flags => ret.replace("FlagBits", "Flags"),
        }
    }
}

impl VkStruct {
    pub fn new(c_struct: CStruct) -> Self {
        let ty = Self::parse_type(&c_struct);
        let new_name = Self::parse_new_name(&c_struct.name, ty);

        Self {
            c_struct,
            ty,
            new_name,
        }
    }

    fn parse_type(c_struct: &CStruct) -> VkStructType {
        if c_struct.fields.first().is_some_and(|it| it.name == "sType") {
            VkStructType::SType
        } else {
            VkStructType::Normal
        }
    }

    fn parse_new_name(name: &str, _ty: VkStructType) -> String {
        let ret = name.replace("Vk", "");
        ret
    }
}

impl Context {
    pub fn new(c_enums: Vec<CEnum>, c_structs: Vec<CStruct>) -> Self {
        let vk_enums = c_enums
            .into_iter()
            .map(|it| (it.name.clone(), Arc::new(VkEnum::new(it))))
            .collect::<BTreeMap<_, _>>();

        let vk_structs = c_structs
            .into_iter()
            .map(|it| (it.name.clone(), Arc::new(VkStruct::new(it))))
            .collect::<BTreeMap<_, _>>();

        Self {
            vk_enums,
            vk_structs,
        }
    }

    /// 在整个解析过程中，一个类型有两种可能：
    ///
    /// 一种是 Vulkan 头文件里定义的类型，即 Vk 类型，
    /// 另一种是该头文件里未定义的类型（原生类型、VkBool32 等），下文称标准类型。
    ///
    /// 因此在 Generator 中会先存一张表，记录哪些类型是头文件里定义的类型。
    /// 如果某个类型名称不在该表中，那么说明它是标准类型。
    ///
    /// 整个过程用查表完成，较快。
    pub fn get_field_new_type_name<'a>(&'a self, type_name: &'a str) -> &'a str {
        if let Some(vk_enum) = self.vk_enums.get(type_name) {
            &vk_enum.new_name
        } else if let Some(vk_struct) = self.vk_structs.get(type_name) {
            &vk_struct.new_name
        } else if let Some(primitive) = PRIMITIVE_MAP.get(type_name) {
            primitive
        } else {
            type_name
        }
    }

    pub fn cast_variable(&self, raw_type_name: &str, new_variable_name: &str) -> String {
        if self.vk_enums.contains_key(raw_type_name) {
            format!("static_cast<{}>({})", raw_type_name, new_variable_name)
        } else if self.vk_structs.contains_key(raw_type_name) {
            format!("{}.raw()", new_variable_name)
        } else if raw_type_name == "VkBool32" {
            format!("static_cast<VkBool32>({})", new_variable_name)
        } else {
            new_variable_name.to_string()
        }
    }
}

fn convert_field_name(field_name: &str) -> String {
    let mut ret = camel_to_snake(&field_name);
    if ret.starts_with("p_") {
        ret.replace_range(..2, "");
    }
    ret
}
