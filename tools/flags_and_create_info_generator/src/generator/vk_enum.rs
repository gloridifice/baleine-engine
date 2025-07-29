use std::collections::HashSet;

use crate::{
    c_parser::CEnum,
    generator::{ElementsGenerator, StringBuilder},
    str_utils::{self, common_prefix_iter},
};

pub struct VkEnumGenerator;

impl ElementsGenerator<CEnum> for VkEnumGenerator {
    fn element(&mut self, element: &CEnum) -> Option<String> {
        if !element.name.starts_with("Vk") {
            return None;
        }

        let CEnum { name, values } = element;

        let mut ret = StringBuilder::new();

        let new_enum_name = name.replace("Vk", "").replace("Bits", "");

        ret.scope(
            &format!("enum class {} : u32", &new_enum_name),
            true,
            |ret| {
                let common_prefix_len = common_prefix_iter(values.iter().map(|it| &it.name));

                // Avoiding add value with same name.
                let mut visited = HashSet::<String>::new();
                for (i, value) in values.iter().enumerate() {
                    // VK_BUFFER_USAGE_INDEX_BUFFER_BIT -> IndexBuffer
                    if visited.contains(&value.name) {
                        continue;
                    }

                    let new_value_name = str_utils::upper_snake_to_upper_camel_iter(
                        &value.name.split_at(common_prefix_len).1.replace("_BIT", ""),
                    );
                    if i != values.len() - 1 {
                        ret.line(&format!("{} = {},", &new_value_name, &value.name));
                    } else {
                        ret.line(&format!("{} = {}", &new_value_name, &value.name));
                    }
                    visited.insert(value.name.clone());
                }
            },
        );

        if name.ends_with("FlagBits") {
            ret.line(&format!("ENABLE_BITMASK_OPERATORS({});", &new_enum_name));
        }

        Some(ret.take_result())
    }
}

#[cfg(test)]
mod test {
    use crate::c_parser::CParser;

    use super::*;

    #[test]
    fn test_enum_generator() {
        let input = include_str!("../../test_files/enum.h");
        let mut parser = CParser::new().unwrap();
        if let Ok((_, enums)) = parser.parse_c_code(input) {
            let result = VkEnumGenerator.generate(&enums);
            println!("{}", &result);
        }
    }

    #[test]
    fn test_repeat() {
        let input = r"
typedef enum VkAttachmentStoreOp {
    VK_ATTACHMENT_STORE_OP_STORE = 0,
    VK_ATTACHMENT_STORE_OP_DONT_CARE = 1,
    VK_ATTACHMENT_STORE_OP_NONE = 1000301000,
    VK_ATTACHMENT_STORE_OP_NONE_KHR = VK_ATTACHMENT_STORE_OP_NONE,
    VK_ATTACHMENT_STORE_OP_NONE_QCOM = VK_ATTACHMENT_STORE_OP_NONE,
    VK_ATTACHMENT_STORE_OP_NONE_EXT = VK_ATTACHMENT_STORE_OP_NONE,
    VK_ATTACHMENT_STORE_OP_MAX_ENUM = 0x7FFFFFFF
} VkAttachmentStoreOp;
        ";
        let mut parser = CParser::new().unwrap();
        if let Ok((_, enums)) = parser.parse_c_code(input) {
            let result = VkEnumGenerator.generate(&enums);
            println!("{}", &result);
        }
    }
}
