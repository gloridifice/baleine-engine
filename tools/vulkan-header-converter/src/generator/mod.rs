use std::{collections::HashMap, sync::LazyLock};

use crate::str_utils::camel_to_snake;

pub mod vk_create_info;
pub mod vk_enum;

pub struct StringBuilder {
    tab_count: u32,
    content: String,
}

impl StringBuilder {
    pub fn new() -> Self {
        Self {
            tab_count: 0,
            content: String::new(),
        }
    }

    pub fn scope(&mut self, before_scope: &str, with_semicolon: bool, fun: impl FnOnce(&mut Self)) {
        match before_scope {
            "" => self.line("{"),
            _ => self.line(&format!("{} {{", before_scope)),
        }
        self.tab();
        fun(self);
        self.end_tab();

        self.line(if with_semicolon { "};" } else { "}" });
    }

    pub fn tab(&mut self) {
        self.tab_count += 1;
    }

    pub fn end_tab(&mut self) {
        if self.tab_count > 0 {
            self.tab_count -= 1;
        }
    }

    pub fn clear_tab(&mut self) {
        self.tab_count = 0;
    }

    pub fn push_str(&mut self, v: &str) {
        self.content.push_str(v);
    }

    /// Auto tab and \n
    pub fn line(&mut self, v: &str) {
        for _ in 0..self.tab_count {
            self.content.push_str("    ");
        }
        self.content.push_str(v);
        self.content.push('\n');
    }

    pub fn take_result(self) -> String {
        self.content
    }

    pub fn clone_result(&self) -> String {
        self.content.clone()
    }
}

pub trait ElementsGenerator<T> {
    fn element(&mut self, element: &T) -> Option<String>;
    fn generate(&mut self, elements: &[T]) -> String {
        let mut ret = String::new();
        for ele in elements.into_iter() {
            if let Some(ele) = self.element(ele) {
                ret.push_str(&ele);
                ret.push('\n');
            }
        }
        ret
    }
}

pub static PRIMITIVE_MAP: LazyLock<HashMap<&'static str, &'static str>> = LazyLock::new(|| {
    [
        ("uint32_t", "u32"),
        ("uint16_t", "u16"),
        ("int32_t", "i32"),
        ("int16_t", "i16"),
        ("bool", "bool"),
        ("int", "i32"),
        ("unsigned int", "u32"),
        ("float", "f32"),
        ("double", "f64"),
    ]
    .into_iter()
    .collect()
});

#[derive(Clone)]
pub enum VkType {
    VkSTypedStruct,
    VkFlagsEnum,
    VkNormalStruct,
    VkNormalEnum,
    VkOthers,
    VkBool32,
    Primitive,
    Others,
}

fn convert_field_name(field_name: &str) -> String {
    let mut ret = camel_to_snake(&field_name);
    if ret.starts_with("p_") {
        ret.replace_range(..2, "");
    }
    ret
}

impl VkType {
    pub fn parse(name: &str) -> Self {
        if PRIMITIVE_MAP.contains_key(name) {
            return VkType::Primitive;
        }
        if name == "VkBool32" {
            return VkType::VkBool32;
        }
        if name.starts_with("Vk") {
            if [
                "Flags",
                "FlagBits",
                "FlagsKHR",
                "FlagBitsKHR",
                "FlagsEXT",
                "FlagBitsEXT",
            ]
            .iter()
            .any(|it| name.ends_with(it))
            {
                return VkType::VkFlagsEnum;
            }
            if [
                "Info",
                "Info2",
                "InfoKHR",
                "InfoEXT",
                "Properties",
                "Features",
            ]
            .iter()
            .any(|it| name.ends_with(it))
            {
                return VkType::VkSTypedStruct;
            }
            return VkType::VkOthers;
        }
        VkType::Others
    }

    pub fn convert(&self, name: &str) -> String {
        use VkType::*;
        match self {
            VkFlagsEnum => name
                .replace("Vk", "")
                .replace("Flags", "Flag")
                .replace("FlagBits", "Flag"),
            Primitive => PRIMITIVE_MAP.get(name).unwrap().to_string(),
            Others => name.to_string(),
            VkOthers => name.to_string(),
            VkBool32 => "bool".to_string(),
            VkSTypedStruct | VkNormalStruct | VkNormalEnum => name.replace("Vk", ""),
        }
    }

    pub fn cast_variable(&self, raw_type_name: &str, new_variable_name: &str) -> String {
        use VkType::*;
        match self {
            VkSTypedStruct | VkNormalStruct => format!("{}.raw()", new_variable_name),
            VkFlagsEnum | VkNormalEnum => {
                format!("static_cast<{}>({})", raw_type_name, new_variable_name)
            }
            VkBool32 => format!("static_cast<VkBool32>({})", new_variable_name),
            _ => new_variable_name.to_string(),
        }
    }
}
