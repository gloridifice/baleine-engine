use crate::{
    c_parser::CStruct,
    generator::{Context, ElementsGenerator, VkStruct, VkStructType, convert_field_name},
    str_utils::{StringBuilder, camel_to_snake},
};

pub struct VkStructGenerator;

impl ElementsGenerator<VkStruct> for VkStructGenerator {
    fn element(&mut self, element: &VkStruct, context: &Context) -> Option<String> {
        let mut ret = StringBuilder::new();

        let VkStruct {
            c_struct:
                CStruct {
                    name: raw_name,
                    fields,
                },
            ty,
            new_name,
        } = element;

        // struct
        ret.scope(&format!("struct {}", &new_name), true, |ret| {
            let iter = match ty {
                VkStructType::Normal => fields.iter().skip(0),
                VkStructType::SType => fields.iter().skip(2),
            };
            let rest_fields = iter
                .map(|it| {
                    let converted_field_type = context.get_field_new_type_name(&it.field_type);
                    let converted_field_name = convert_field_name(&it.name);
                    (it, converted_field_type, converted_field_name)
                })
                .collect::<Vec<_>>();

            for (_, converted_type, converted_name) in rest_fields.iter() {
                ret.line(&format!("{} {};", &converted_type, &converted_name));
            }

            ret.line("");

            // raw() function
            ret.scope(&format!("{} raw()", &raw_name), false, |ret| {
                // construct VK struct
                ret.scope(&format!("return {}", &raw_name), true, |ret| {
                    if *ty == VkStructType::SType {
                        ret.line(&format!(".sType = {},", cast_s_type(&raw_name)));
                        ret.line(".pNext = nullptr,");
                    }

                    for (i, (field, _, converted_field_name)) in rest_fields.iter().enumerate() {
                        let value = context.cast_variable(&field.field_type, &converted_field_name);
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
