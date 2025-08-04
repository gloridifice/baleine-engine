pub fn upper_snake_to_upper_camel_iter(input: &str) -> String {
    input
        .split('_')
        .filter(|s| !s.is_empty())
        .map(|word| {
            let mut chars = word.chars();
            match chars.next() {
                Some(c) => c
                    .to_uppercase()
                    .chain(chars.flat_map(|c| c.to_lowercase()))
                    .collect(),
                None => String::new(),
            }
        })
        .collect()
}

pub fn common_prefix_iter<I, S>(mut iter: I) -> usize
where
    I: Iterator<Item = S>,
    S: AsRef<str>,
{
    // Get the first string as reference
    let first = match iter.next() {
        Some(s) => s.as_ref().to_string(),
        None => return 0, // Empty iterator
    };

    // Collect remaining strings to compare against
    let others: Vec<String> = iter.map(|s| s.as_ref().to_string()).collect();

    if others.is_empty() {
        return 0; // Single item
    }

    // Find common prefix
    let min_len = others
        .iter()
        .map(|s| s.len())
        .min()
        .unwrap_or(0)
        .min(first.len());

    let mut prefix_len = 0;

    for i in 0..min_len {
        let first_byte = first.as_bytes()[i];

        if others.iter().all(|s| s.as_bytes()[i] == first_byte) {
            prefix_len = i + 1;
        } else {
            break;
        }
    }

    prefix_len
}

pub fn camel_to_snake(input: &str) -> String {
    let mut result = String::new();

    let mut last_char: Option<char> = None;
    for (i, ch) in input.chars().enumerate() {
        if ch.is_uppercase() && last_char.is_some_and(|it| it.is_lowercase()) && i > 0 {
            result.push('_');
        }
        last_char = Some(ch);
        result.push(ch.to_lowercase().next().unwrap());
    }

    result
}

/// Helper struct for buiding Cpp style String
pub struct StringBuilder {
    tab_count: u32,
    content: String,
}

impl StringBuilder {
    pub fn new() -> Self {
        Self {
            tab_count: 0,
            content: String::new(),
        }
    }

    pub fn scope(&mut self, before_scope: &str, with_semicolon: bool, fun: impl FnOnce(&mut Self)) {
        match before_scope {
            "" => self.line("{"),
            _ => self.line(&format!("{} {{", before_scope)),
        }
        self.tab();
        fun(self);
        self.end_tab();

        self.line(if with_semicolon { "};" } else { "}" });
    }

    pub fn tab(&mut self) {
        self.tab_count += 1;
    }

    pub fn end_tab(&mut self) {
        if self.tab_count > 0 {
            self.tab_count -= 1;
        }
    }

    pub fn clear_tab(&mut self) {
        self.tab_count = 0;
    }

    pub fn push_str(&mut self, v: &str) {
        self.content.push_str(v);
    }

    /// Auto tab and \n
    pub fn line(&mut self, v: &str) {
        for _ in 0..self.tab_count {
            self.content.push_str("    ");
        }
        self.content.push_str(v);
        self.content.push('\n');
    }

    pub fn take_result(self) -> String {
        self.content
    }

    pub fn clone_result(&self) -> String {
        self.content.clone()
    }
}
