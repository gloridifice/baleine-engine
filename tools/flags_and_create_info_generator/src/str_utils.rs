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

    for (i, ch) in input.chars().enumerate() {
        if ch.is_uppercase() && i > 0 {
            result.push('_');
        }
        result.push(ch.to_lowercase().next().unwrap());
    }

    result
}
