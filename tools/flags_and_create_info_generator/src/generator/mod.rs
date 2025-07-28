use crate::c_parser::*;
use std::collections::HashMap;

pub mod vk_create_info;
pub mod vk_enum;

pub trait ElementsGenerator<T> {
    fn element(&mut self, element: &T) -> String;
    fn generate(&mut self, elements: &[T]) -> String {
        let mut ret = String::new();
        for ele in elements {
            ret.push_str(&self.element(ele));
            ret.push('\n');
        }
        ret
    }
}

pub struct CppGenerator {
    type_mappings: HashMap<String, String>,
}

impl CppGenerator {
    pub fn new() -> Self {
        let mut type_mappings = HashMap::new();
        type_mappings.insert("char".to_string(), "char".to_string());
        type_mappings.insert("int".to_string(), "int".to_string());
        type_mappings.insert("float".to_string(), "float".to_string());
        type_mappings.insert("double".to_string(), "double".to_string());
        type_mappings.insert("void".to_string(), "void".to_string());

        CppGenerator { type_mappings }
    }

    pub fn generate_cpp_structs(&self, structs: &[CStruct]) -> String {
        let mut cpp_code = String::new();
        cpp_code.push_str("#include <string>\n");
        cpp_code.push_str("#include <vector>\n");
        cpp_code.push_str("#include <array>\n");
        cpp_code.push_str("#include <memory>\n\n");

        for c_struct in structs {
            cpp_code.push_str(&self.convert_struct(c_struct));
            cpp_code.push_str("\n\n");
        }

        cpp_code
    }

    pub fn generate_cpp_enums(&self, enums: &[CEnum]) -> String {
        let mut cpp_code = String::new();

        for c_enum in enums {
            cpp_code.push_str(&self.convert_enum(c_enum));
            cpp_code.push_str("\n\n");
        }

        cpp_code
    }

    fn convert_struct(&self, c_struct: &CStruct) -> String {
        let mut cpp_struct = String::new();

        cpp_struct.push_str(&format!("struct {} {{\n", c_struct.name));

        for field in &c_struct.fields {
            cpp_struct.push_str("    ");
            cpp_struct.push_str(&self.convert_field(field));
            cpp_struct.push_str(";\n");
        }

        // Add constructor
        if !c_struct.fields.is_empty() {
            cpp_struct.push_str("\n    // Constructor\n");
            cpp_struct.push_str(&format!("    {}() = default;\n", c_struct.name));

            // Parameterized constructor
            cpp_struct.push_str(&format!("    {}(", c_struct.name));
            let params: Vec<String> = c_struct
                .fields
                .iter()
                .map(|f| format!("const {}& {}", self.get_cpp_field_type(f), f.name))
                .collect();
            cpp_struct.push_str(&params.join(", "));
            cpp_struct.push_str(")\n        : ");

            let initializers: Vec<String> = c_struct
                .fields
                .iter()
                .map(|f| format!("{}({})", f.name, f.name))
                .collect();
            cpp_struct.push_str(&initializers.join(", "));
            cpp_struct.push_str(" {}\n");
        }

        cpp_struct.push_str("};");
        cpp_struct
    }

    fn convert_field(&self, field: &StructField) -> String {
        let cpp_type = self.get_cpp_field_type(field);
        format!("{} {}", cpp_type, field.name)
    }

    fn get_cpp_field_type(&self, field: &StructField) -> String {
        let base_type = self
            .type_mappings
            .get(&field.field_type)
            .cloned()
            .unwrap_or_else(|| match field.field_type.as_str() {
                "char" if field.is_pointer => "std::string".to_string(),
                _ => field.field_type.clone(),
            });

        if field.is_array {
            if let Some(ref size) = field.array_size {
                format!("std::array<{}, {}>", base_type, size)
            } else {
                format!("std::vector<{}>", base_type)
            }
        } else if field.is_pointer && field.field_type != "char" {
            format!("std::unique_ptr<{}>", base_type)
        } else {
            base_type
        }
    }

    fn convert_enum(&self, c_enum: &CEnum) -> String {
        let mut cpp_enum = String::new();

        cpp_enum.push_str(&format!("enum class {} {{\n", c_enum.name));

        for (i, enum_value) in c_enum.values.iter().enumerate() {
            cpp_enum.push_str("    ");
            cpp_enum.push_str(&enum_value.name);

            if let Some(ref value) = enum_value.value {
                cpp_enum.push_str(&format!(" = {}", value));
            }

            if i < c_enum.values.len() - 1 {
                cpp_enum.push(',');
            }
            cpp_enum.push('\n');
        }

        cpp_enum.push_str("};");
        cpp_enum
    }
}
