use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs;
use std::path::Path;
use std::process::Command;

#[derive(Debug, Deserialize, Serialize)]
struct DependencySpec {
    url: String,
    tag: Option<String>,
    branch: Option<String>,
    exclude_from_all: Option<bool>,
}

#[derive(Debug, Deserialize)]
struct Config {
    dependencies: HashMap<String, DependencySpec>,
}

pub fn collect_dependencies(cover_old_files: bool) -> anyhow::Result<()> {
    // Read the TOML file
    let toml_content = fs::read_to_string("Dependencies.toml")?;
    let config: Config = toml::from_str(&toml_content)?;

    // Create third_party directory
    let third_party_dir = Path::new("engine/third_party/auto");
    if !third_party_dir.exists() {
        fs::create_dir_all(third_party_dir)?;
    }

    let mut cmake_content = String::new();
    cmake_content.push_str("# Auto-generated CMakeLists.txt\n");
    cmake_content.push_str("cmake_minimum_required(VERSION 3.31)\n\n");

    // Clone each dependency
    for (name, spec) in &config.dependencies {
        println!("Processing dependency: {}", name);

        let target_dir = third_party_dir.join(name);

        if cover_old_files {
            println!("Removing existing directory: {}", target_dir.display());
            fs::remove_dir_all(&target_dir)?;
        }

        // Remove existing directory if it exists
        if !target_dir.exists() {
            // Clone the repository
            let mut git_cmd = Command::new("git");
            git_cmd.args(&["clone", &spec.url, target_dir.to_str().unwrap()]);

            // Add tag or branch specification
            if let Some(tag) = &spec.tag {
                git_cmd.args(&["--branch", tag]);
                println!("Cloning {} with tag: {}", name, tag);
            } else if let Some(branch) = &spec.branch {
                git_cmd.args(&["--branch", branch]);
                println!("Cloning {} with branch: {}", name, branch);
            } else {
                println!("Cloning {} from default branch", name);
            }

            // Execute git clone
            let output = git_cmd.output()?;

            if !output.status.success() {
                eprintln!(
                    "Failed to clone {}: {}",
                    name,
                    String::from_utf8_lossy(&output.stderr)
                );
                let _ = fs::remove_dir_all(&target_dir.join(name));
                continue;
            }

            println!("Successfully cloned {}", name);
        } else {
            println!("Skipping existing directory: {}", target_dir.display());
        }

        // Check if the dependency has a CMakeLists.txt file
        let cmake_file = target_dir.join("CMakeLists.txt");
        let details = if spec.exclude_from_all.unwrap_or_default() {
            " EXCLUDE_FROM_ALL"
        } else {
            ""
        };
        if cmake_file.exists() {
            cmake_content.push_str(&format!("# Add subdirectory for {}\n", name));
            cmake_content.push_str(&format!("add_subdirectory({}{})\n\n", name, details));
        } else {
            cmake_content.push_str(&format!(
                "# Note: {} does not have CMakeLists.txt - manual configuration may be needed\n",
                name
            ));
            cmake_content.push_str(&format!("# add_subdirectory({}{})\n\n", name, details));
        }
    }

    // Write CMakeLists.txt to third_party directory
    let cmake_path = third_party_dir.join("CMakeLists.txt");
    fs::write(&cmake_path, cmake_content)?;
    println!("Generated CMakeLists.txt at: {}", cmake_path.display());

    println!("All dependencies processed successfully!");
    Ok(())
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_toml_parsing() {
        let toml_content = r#"
[dependencies]
fmt = { url = "https://github.com/fmtlib/fmt.git", tag = "11.2.0" }
spdlog = { url = "https://github.com/gabime/spdlog.git", branch = "v1.x" }
        "#;

        let config: Config = toml::from_str(toml_content).unwrap();
        assert_eq!(config.dependencies.len(), 2);
        assert!(config.dependencies.contains_key("fmt"));
        assert!(config.dependencies.contains_key("spdlog"));

        let fmt_spec = &config.dependencies["fmt"];
        assert_eq!(fmt_spec.url, "https://github.com/fmtlib/fmt.git");
        assert_eq!(fmt_spec.tag, Some("11.2.0".to_string()));
        assert_eq!(fmt_spec.branch, None);
    }
}
