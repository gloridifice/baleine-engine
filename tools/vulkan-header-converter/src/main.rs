use std::{fs::File, io::Write};

use vulkan_header_converter::{c_parser::CParser, generator::Generator};

fn main() {
    let mut parser = CParser::new().unwrap();
    let (a, b) = parser
        .parse_c_code(include_str!("../test_files/all.h"))
        .unwrap();
    let generator = Generator::new(b, a);
    let result = generator.generate();
    File::create("test_files/output.h")
        .unwrap()
        .write_all(result.as_bytes())
        .unwrap();
}
