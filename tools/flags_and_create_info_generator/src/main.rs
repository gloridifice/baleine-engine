use flags_and_create_info_generator::{c_parser::*, generator::CppGenerator};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let c_code = r#"
        typedef enum Status {
            SUCCESS = 0,
            ERROR = 1,
            PENDING = 2
        } Status;

        typedef enum Color {
            RED,
            GREEN,
            BLUE
        };

        struct Point {
            int x;
            int y;
        };

        struct Person {
            char* name;
            int age;
            float height;
            int scores[10];
            struct Point* location;
        };

        struct ComplexStruct {
            char title[256];
            double* values;
            enum Status status;
            struct Person people[5];
        };
    "#;

    let mut parser = CStructEnumParser::new()?;
    let (structs, enums) = parser.parse_c_code(c_code)?;

    println!(
        "Parsed {} structs and {} enums:",
        structs.len(),
        enums.len()
    );

    for c_struct in &structs {
        println!("Struct: {}", c_struct.name);
        for field in &c_struct.fields {
            println!(
                "  - {} {}{}{}",
                field.field_type,
                if field.is_pointer { "*" } else { "" },
                field.name,
                if field.is_array {
                    if let Some(ref size) = field.array_size {
                        format!("[{}]", size)
                    } else {
                        "[]".to_string()
                    }
                } else {
                    String::new()
                }
            );
        }
    }

    for c_enum in &enums {
        println!("Enum: {}", c_enum.name);
        for value in &c_enum.values {
            if let Some(ref val) = value.value {
                println!("  - {} = {}", value.name, val);
            } else {
                println!("  - {}", value.name);
            }
        }
    }

    let generator = CppGenerator::new();

    println!("\n=== Generated C++ Enums ===");
    println!("{}", generator.generate_cpp_enums(&enums));

    println!("=== Generated C++ Structs ===");
    println!("{}", generator.generate_cpp_structs(&structs));

    Ok(())
}
