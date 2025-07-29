use crate::{
    c_parser::CStruct,
    generator::{ElementsGenerator, StringBuilder, VkType, convert_field_name, parse_type_name},
    str_utils::camel_to_snake,
};

pub struct VkCreateInfoGenerator;

impl ElementsGenerator<CStruct> for VkCreateInfoGenerator {
    fn element(&mut self, element: &CStruct) -> Option<String> {
        let mut ret = StringBuilder::new();
        let CStruct { name, fields } = element;
        if !name.starts_with("Vk") || !name.ends_with("CreateInfo") {
            return None;
        }

        let VkType::VkCreateInfo = parse_type_name(&element.name) else {
            return None;
        };

        let new_name = VkType::VkCreateInfo.convert(&name);

        // struct
        ret.scope(&format!("struct {}", &new_name), true, |ret| {
            let rest_fields = fields
                .iter()
                .skip(2)
                .map(|it| {
                    let ty = parse_type_name(&it.field_type);
                    let converted_field_type = ty.convert(&it.field_type);
                    let converted_field_name = convert_field_name(&it.name);
                    (it, ty, converted_field_type, converted_field_name)
                })
                .collect::<Vec<_>>();

            for (_, _, converted_type, converted_name) in rest_fields.iter() {
                ret.line(&format!("{} {};", &converted_type, &converted_name));
            }

            ret.line("");

            // raw() function
            ret.scope(&format!("{} raw()", &name), false, |ret| {
                // construct VK struct
                ret.scope(&format!("return {}", &name), true, |ret| {
                    ret.line(&format!(".sType = {},", cast_s_type(&name))); //TODO
                    ret.line(".pNext = nullptr,");

                    for (i, (field, ty, _, converted_field_name)) in rest_fields.iter().enumerate()
                    {
                        let value = ty.cast_variable(&field.field_type, &converted_field_name);
                        if i != rest_fields.len() - 1 {
                            ret.line(&format!(".{} = {},", &field.name, &value));
                        } else {
                            ret.line(&format!(".{} = {}", &field.name, &value));
                        }
                    }
                });
            });
        });

        Some(ret.take_result())
    }
}

fn cast_s_type(name: &str) -> String {
    format!(
        "VK_STRUCT_TYPE_{}",
        camel_to_snake(&name.replace("Vk", "")).to_uppercase()
    )
}

#[cfg(test)]
mod test {
    use super::*;
    use crate::c_parser::CParser;

    #[test]
    fn test_generator() {
        let s = include_str!("../../test_files/create_info.h");
        let mut parser = CParser::new().unwrap();
        let result = parser.parse_c_code(&s).unwrap();
        let output = VkCreateInfoGenerator.generate(&result.0);
        println!("{}", &output);
    }
}
