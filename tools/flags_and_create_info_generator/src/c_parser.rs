use tree_sitter::{Node, Parser};

#[derive(Debug, Clone)]
pub struct CStruct {
    pub name: String,
    pub fields: Vec<StructField>,
}

#[derive(Debug, Clone)]
pub struct StructField {
    pub name: String,
    pub field_type: String,
    pub is_pointer: bool,
    pub is_array: bool,
    pub array_size: Option<String>,
}

#[derive(Debug, Clone)]
pub struct CEnum {
    pub name: String,
    pub values: Vec<EnumValue>,
}

#[derive(Debug, Clone)]
pub struct EnumValue {
    pub name: String,
    pub value: Option<String>,
}

pub struct CStructEnumParser {
    parser: Parser,
}

impl CStructEnumParser {
    pub fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let mut parser = Parser::new();
        let language = tree_sitter_c::language();
        parser.set_language(language)?;
        Ok(CStructEnumParser { parser })
    }

    pub fn parse_c_code(
        &mut self,
        source_code: &str,
    ) -> Result<(Vec<CStruct>, Vec<CEnum>), Box<dyn std::error::Error>> {
        let tree = self.parser.parse(source_code, None).unwrap();
        let root_node = tree.root_node();

        let mut structs = Vec::new();
        let mut enums = Vec::new();

        self.extract_declarations(root_node, source_code, &mut structs, &mut enums);

        Ok((structs, enums))
    }

    fn extract_declarations(
        &self,
        node: Node,
        source_code: &str,
        structs: &mut Vec<CStruct>,
        enums: &mut Vec<CEnum>,
    ) {
        match node.kind() {
            "struct_specifier" => {
                if let Some(c_struct) = self.parse_struct(node, source_code) {
                    structs.push(c_struct);
                }
            }
            "enum_specifier" => {
                if let Some(c_enum) = self.parse_enum(node, source_code) {
                    enums.push(c_enum);
                }
            }
            _ => {
                // Recursively check children
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.extract_declarations(child, source_code, structs, enums);
                    }
                }
            }
        }
    }

    fn parse_struct(&self, node: Node, source_code: &str) -> Option<CStruct> {
        let mut struct_name = String::new();
        let mut fields = Vec::new();

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "type_identifier" => {
                        struct_name = self.node_text(child, source_code);
                    }
                    "field_declaration_list" => {
                        fields = self.parse_field_declaration_list(child, source_code);
                    }
                    _ => {}
                }
            }
        }

        if !struct_name.is_empty() {
            Some(CStruct {
                name: struct_name,
                fields,
            })
        } else {
            None
        }
    }

    fn parse_field_declaration_list(&self, node: Node, source_code: &str) -> Vec<StructField> {
        let mut fields = Vec::new();

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "field_declaration" {
                    if let Some(field) = self.parse_field_declaration(child, source_code) {
                        fields.push(field);
                    }
                }
            }
        }

        fields
    }

    fn parse_field_declaration(&self, node: Node, source_code: &str) -> Option<StructField> {
        let mut field_type = String::new();
        let mut field_name = String::new();
        let mut is_pointer = false;
        let mut is_array = false;
        let mut array_size = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "primitive_type" | "type_identifier" => {
                        field_type = self.node_text(child, source_code);
                    }
                    "field_declarator" => {
                        let (name, ptr, arr, size) =
                            self.parse_field_declarator(child, source_code);
                        field_name = name;
                        is_pointer = ptr;
                        is_array = arr;
                        array_size = size;
                    }
                    _ => {}
                }
            }
        }

        if !field_name.is_empty() && !field_type.is_empty() {
            Some(StructField {
                name: field_name,
                field_type,
                is_pointer,
                is_array,
                array_size,
            })
        } else {
            None
        }
    }

    fn parse_field_declarator(
        &self,
        node: Node,
        source_code: &str,
    ) -> (String, bool, bool, Option<String>) {
        let mut name = String::new();
        let mut is_pointer = false;
        let mut is_array = false;
        let mut array_size = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "identifier" => {
                        name = self.node_text(child, source_code);
                    }
                    "pointer_declarator" => {
                        is_pointer = true;
                        // Recursively parse the pointer declarator
                        let (inner_name, _, inner_arr, inner_size) =
                            self.parse_field_declarator(child, source_code);
                        if !inner_name.is_empty() {
                            name = inner_name;
                        }
                        is_array = inner_arr;
                        array_size = inner_size;
                    }
                    "array_declarator" => {
                        is_array = true;
                        let (inner_name, inner_ptr, _, inner_size) =
                            self.parse_array_declarator(child, source_code);
                        if !inner_name.is_empty() {
                            name = inner_name;
                        }
                        is_pointer = inner_ptr;
                        array_size = inner_size;
                    }
                    _ => {}
                }
            }
        }

        (name, is_pointer, is_array, array_size)
    }

    fn parse_array_declarator(
        &self,
        node: Node,
        source_code: &str,
    ) -> (String, bool, bool, Option<String>) {
        let mut name = String::new();
        let mut is_pointer = false;
        let mut array_size = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "identifier" => {
                        name = self.node_text(child, source_code);
                    }
                    "pointer_declarator" => {
                        is_pointer = true;
                        let (inner_name, _, _, _) = self.parse_field_declarator(child, source_code);
                        if !inner_name.is_empty() {
                            name = inner_name;
                        }
                    }
                    "number_literal" => {
                        array_size = Some(self.node_text(child, source_code));
                    }
                    _ => {}
                }
            }
        }

        (name, is_pointer, true, array_size)
    }

    fn parse_enum(&self, node: Node, source_code: &str) -> Option<CEnum> {
        let mut enum_name = String::new();
        let mut values = Vec::new();

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "type_identifier" => {
                        enum_name = self.node_text(child, source_code);
                    }
                    "enumerator_list" => {
                        values = self.parse_enumerator_list(child, source_code);
                    }
                    _ => {}
                }
            }
        }

        if !enum_name.is_empty() {
            Some(CEnum {
                name: enum_name,
                values,
            })
        } else {
            None
        }
    }

    fn parse_enumerator_list(&self, node: Node, source_code: &str) -> Vec<EnumValue> {
        let mut values = Vec::new();

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "enumerator" {
                    if let Some(enum_value) = self.parse_enumerator(child, source_code) {
                        values.push(enum_value);
                    }
                }
            }
        }

        values
    }

    fn parse_enumerator(&self, node: Node, source_code: &str) -> Option<EnumValue> {
        let mut name = String::new();
        let mut value = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "identifier" => {
                        name = self.node_text(child, source_code);
                    }
                    "number_literal" => {
                        value = Some(self.node_text(child, source_code));
                    }
                    _ => {}
                }
            }
        }

        if !name.is_empty() {
            Some(EnumValue { name, value })
        } else {
            None
        }
    }

    fn node_text(&self, node: Node, source_code: &str) -> String {
        source_code[node.start_byte()..node.end_byte()].to_string()
    }
}
