use regex::Regex;

pub struct CEnum {
    pub name: String,
    pub ident_values: Vec<(String, String)>,
}

pub fn parse_c_enums(content: &str) -> anyhow::Result<Vec<CEnum>> {
    // 编译正则表达式来匹配 C 风格枚举
    let enum_regex = Regex::new(
        r"(?x)
        enum\s+                 # enum 关键字
        (?P<name>\w+)           # 枚举名称
        \s*                     # 可选空格
        \{\s*                   # 开始大括号
        (?P<fields>[^}]*)       # 枚举字段（排除结束大括号）
        \}\s*;                  # 结束大括号和分号
    ",
    )?;

    let c_enums = enum_regex
        .captures_iter(&content)
        .map(|cap| {
            let name = &cap["name"];
            let fields = &cap["fields"];

            let mut vec = Vec::<(String, String)>::new();
            for field in fields.split(',') {
                let trimmed_field = field.trim();

                if trimmed_field.is_empty() {
                    continue;
                }

                if let Some((ident, value)) = trimmed_field.split_once('=') {
                    let clean_ident = ident.trim();
                    let clean_value = value.trim().trim_end_matches(',').trim();
                    vec.push((clean_ident.to_string(), clean_value.to_string()));
                }
            }

            CEnum {
                name: name.to_string(),
                ident_values: vec,
            }
        })
        .collect::<Vec<_>>();

    Ok(c_enums)
}
