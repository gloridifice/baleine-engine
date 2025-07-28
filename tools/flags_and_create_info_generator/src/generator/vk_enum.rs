use crate::{c_parser::CEnum, generator::ElementsGenerator, namming_convention};

pub struct VkEnumGenerator;

impl ElementsGenerator<CEnum> for VkEnumGenerator {
    fn element(&mut self, element: &CEnum) -> String {
        let mut ret = String::new();
        ret.push_str(&format!("enum class {} {{\n", &element.name));
        for (i, value) in element.values.iter().enumerate() {
            let new_value_name = namming_convention::upper_snake_to_upper_camel_iter(&value.name);
            let new_value = &value.name;
            if i != element.values.len() - 1 {
                ret.push_str(&format!("{} = {},\n", &new_value_name, &new_value));
            } else {
                ret.push_str(&format!("{} = {}\n", &new_value_name, &new_value));
            }
        }
        ret.push_str("}}\n");
        ret
    }
}
