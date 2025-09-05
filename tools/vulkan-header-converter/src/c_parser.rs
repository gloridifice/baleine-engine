use tree_sitter::{Node, Parser};

#[derive(Debug, Clone)]
pub struct CStruct {
    pub name: String,
    pub fields: Vec<CField>,
}

#[derive(Debug, Clone)]
pub struct CField {
    pub field_type: String,
    pub name: String,
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

pub struct CParser {
    parser: Parser,
}

impl CParser {
    pub fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let mut parser = Parser::new();
        let language = tree_sitter_c::language();
        parser.set_language(language)?;
        Ok(CParser { parser })
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

    // ================== Struct ==============================

    // Helper function to debug node structure
    #[allow(unused)]
    fn print_node_structure(&self, node: Node, source_code: &str, depth: usize) {
        let indent = "  ".repeat(depth);
        println!(
            "{}Node: {} = '{}'",
            indent,
            node.kind(),
            self.node_text(node, source_code)
        );

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.print_node_structure(child, source_code, depth + 1);
            }
        }
    }

    fn node_text(&self, node: Node, source_code: &str) -> String {
        source_code[node.start_byte()..node.end_byte()].to_string()
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
                    _ => {
                        println!("Unhandled child kind: {}", child.kind());
                    }
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

    fn parse_field_declaration_list(&self, node: Node, source_code: &str) -> Vec<CField> {
        let mut fields = Vec::new();

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "field_declaration" => {
                        if let Some(field) = self.parse_field_declaration(child, source_code) {
                            fields.push(field);
                        }
                    }
                    "{" | "}" => {
                        // Skip braces
                    }
                    _ => {
                        println!("Unhandled field list child: {}", child.kind());
                    }
                }
            }
        }

        fields
    }

    fn parse_field_declaration(&self, node: Node, source_code: &str) -> Option<CField> {
        let mut field_type = String::new();
        let mut field_name = String::new();

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    // Basic types
                    "primitive_type" => {
                        field_type = self.node_text(child, source_code);
                    }
                    // User-defined types
                    "type_identifier" => {
                        if field_type.is_empty() {
                            field_type = self.node_text(child, source_code);
                        }
                    }
                    // Field name
                    "field_identifier" => {
                        field_name = self.node_text(child, source_code);
                    }
                    // Sometimes the field name is just an identifier
                    "identifier" => {
                        if field_name.is_empty() {
                            field_name = self.node_text(child, source_code);
                        }
                    }
                    // Handle pointer declarations
                    "pointer_declarator" => {
                        if let Some(pointer_info) =
                            self.parse_pointer_declarator(child, source_code)
                        {
                            field_type.push_str(&pointer_info.0); // Add pointer stars
                            if field_name.is_empty() {
                                field_name = pointer_info.1; // Get the identifier
                            }
                        }
                    }
                    // Handle array declarations
                    "array_declarator" => {
                        if let Some(array_info) = self.parse_array_declarator(child, source_code) {
                            field_name = array_info.0;
                            field_type.push_str(&array_info.1); // Add array brackets
                        }
                    }
                    ";" => {
                        // Skip semicolon
                    }
                    _ => {
                        println!("Unhandled field declaration child: {}", child.kind());
                    }
                }
            }
        }

        if !field_type.is_empty() && !field_name.is_empty() {
            Some(CField {
                field_type: field_type.trim().to_string(),
                name: field_name.trim().to_string(),
            })
        } else {
            println!(
                "Failed to parse field - type: '{}', name: '{}'",
                field_type, field_name
            );
            None
        }
    }

    fn parse_pointer_declarator(&self, node: Node, source_code: &str) -> Option<(String, String)> {
        let mut stars = String::new();
        let mut identifier = String::new();

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "*" => {
                        stars.push('*');
                    }
                    "identifier" | "field_identifier" => {
                        identifier = self.node_text(child, source_code);
                    }
                    "pointer_declarator" => {
                        // Nested pointer
                        if let Some((nested_stars, nested_id)) =
                            self.parse_pointer_declarator(child, source_code)
                        {
                            stars.push_str(&nested_stars);
                            identifier = nested_id;
                        }
                    }
                    _ => {}
                }
            }
        }

        if !identifier.is_empty() {
            Some((stars, identifier))
        } else {
            None
        }
    }

    fn parse_array_declarator(&self, node: Node, source_code: &str) -> Option<(String, String)> {
        let mut identifier = String::new();
        let mut array_part = String::new();

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "identifier" | "field_identifier" => {
                        identifier = self.node_text(child, source_code);
                    }
                    "[" => {
                        array_part.push('[');
                    }
                    "]" => {
                        array_part.push(']');
                    }
                    "number_literal" => {
                        array_part.push_str(&self.node_text(child, source_code));
                    }
                    _ => {}
                }
            }
        }

        if !identifier.is_empty() {
            Some((identifier, array_part))
        } else {
            None
        }
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
}
