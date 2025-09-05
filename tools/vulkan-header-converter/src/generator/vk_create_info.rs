use crate::{
    c_parser::CStruct,
    generator::{ElementsGenerator, StringBuilder, VkType, convert_field_name},
    str_utils::camel_to_snake,
};

pub struct VkStructGenerator;

impl ElementsGenerator<CStruct> for VkStructGenerator {
    fn element(&mut self, element: &CStruct) -> Option<String> {
        let mut ret = StringBuilder::new();
        let CStruct { name, fields } = element;

        let ty = VkType::parse(&element.name);
        let new_name = ty.convert(&name);
        match ty {
            VkType::VkSTypedStruct | VkType::VkNormalStruct => {
                // struct
                ret.scope(&format!("struct {}", &new_name), true, |ret| {
                    let iter = if let VkType::VkSTypedStruct = ty {
                        fields.iter().skip(2)
                    } else {
                        fields.iter().skip(0)
                    };
                    let rest_fields = iter
                        .map(|it| {
                            let ty = VkType::parse(&it.field_type);
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
                            if let VkType::VkSTypedStruct = ty {
                                ret.line(&format!(".sType = {},", cast_s_type(&name)));
                                ret.line(".pNext = nullptr,");
                            }

                            for (i, (field, ty, _, converted_field_name)) in
                                rest_fields.iter().enumerate()
                            {
                                let value =
                                    ty.cast_variable(&field.field_type, &converted_field_name);
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
            _ => None,
        }
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
        let s = include_str!("../../test_files/struct.h");
        let mut parser = CParser::new().unwrap();
        let result = parser.parse_c_code(&s).unwrap();
        let output = VkStructGenerator.generate(&result.0);
        println!("{}", &output);
    }
}
