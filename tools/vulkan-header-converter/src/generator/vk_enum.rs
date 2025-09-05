use std::collections::HashSet;

use crate::{
    c_parser::CEnum,
    generator::{Context, ElementsGenerator, VkEnum, VkEnumType},
    str_utils::{self, StringBuilder, common_prefix_iter},
};

pub struct VkEnumGenerator;

impl ElementsGenerator<VkEnum> for VkEnumGenerator {
    fn element(&mut self, element: &VkEnum, _context: &Context) -> Option<String> {
        let VkEnum {
            c_enum: CEnum { values, .. },
            ty,
            new_name,
        } = element;

        let mut ret = StringBuilder::new();

        ret.scope(&format!("enum class {} : u32", &new_name), true, |ret| {
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
        });

        if *ty == VkEnumType::Flags {
            ret.line(&format!("ENABLE_BITMASK_OPERATORS({});", &new_name));
        }

        Some(ret.take_result())
    }
}
