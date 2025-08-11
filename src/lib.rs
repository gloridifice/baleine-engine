use std::{
    env, fs,
    io::{BufRead, BufReader},
    process::{Command, Stdio},
    thread::spawn,
};

use crate::dependency::collect_dependencies;
use clap::{Parser, Subcommand};
use colored::Colorize;

pub mod dependency;

#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct AppArgs {
    #[command(subcommand)]
    command: AppCommand,
}

#[derive(Subcommand, Debug)]
enum AppCommand {
    /// Download dependencies and build/rebuild cmake project
    Setup {
        /// If remove old dependencies' folders and re-download.
        #[arg(short, long, default_value_t = false)]
        cover_old_files: bool,
    },
    /// Rebuild cmake project
    Rebuild,
    /// Re-Collect dependencies
    Dependencies {
        /// If remove old dependencies' folders and re-download.
        #[arg(short, long, default_value_t = false)]
        cover_old_files: bool,
    },
}

fn run_output_command(command: &mut Command) -> anyhow::Result<()> {
    let mut child = command
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn()?;

    let stdout = child.stdout.take().unwrap();
    let stderr = child.stderr.take().unwrap();

    let stdout_task = spawn(move || {
        let mut reader = BufReader::new(stdout).lines();
        while let Some(Ok(line)) = reader.next() {
            println!("{}", &line);
        }
    });

    let stderr_task = spawn(move || {
        let mut reader = BufReader::new(stderr).lines();
        while let Some(Ok(line)) = reader.next() {
            println!("{}", &line);
        }
    });

    stdout_task.join().unwrap();
    stderr_task.join().unwrap();

    let result = child.wait()?;
    if !result.success() {
        println!("Failed to run command, code: <{:?}>.", result.code());
    }

    Ok(())
}

fn rebuild_cmake_project() -> anyhow::Result<()> {
    let env_dir = env::current_dir()?;
    let build_dir_path = env_dir.join("engine/build");
    let cache_dir_path = env_dir.join("engine/.cache");
    if build_dir_path.exists() {
        fs::remove_dir_all(&build_dir_path)?;
    }
    if cache_dir_path.exists() {
        fs::remove_dir_all(&cache_dir_path)?;
    }
    if !build_dir_path.exists() {
        fs::create_dir_all(&build_dir_path)?;
    }

    let mut command = Command::new("cmake");
    command
        .current_dir(&build_dir_path)
        .args(["..", "-G", "Ninja"].into_iter());

    run_output_command(&mut command)
}

pub fn log_reuslt_colored_string(result: &anyhow::Result<()>, behavior: &str) -> String {
    match result {
        Ok(_) => {
            format!(
                "{} {} {}",
                "INFO - Succeffully".green(),
                behavior.bright_green(),
                "!".green()
            )
        }
        Err(e) => {
            format!(
                "{} {} {}",
                "ERROR - Failed to".red(),
                behavior.bright_red(),
                format!("! Err: {:?}", e).red(),
            )
        }
    }
}

pub fn run_mission(fun: impl Fn() -> anyhow::Result<()>, behavior: &str) {
    println!(
        "{}",
        format!(
            "{} {} {}",
            "INFO - Begin to".magenta(),
            behavior.bright_magenta(),
            "!".magenta()
        )
    );
    let result = fun();
    println!("{}", log_reuslt_colored_string(&result, behavior));
}

pub fn run() {
    let args = AppArgs::parse();

    match args.command {
        AppCommand::Setup { cover_old_files } => {
            run_mission(
                || collect_dependencies(cover_old_files),
                "collect dependencies",
            );

            run_mission(
                || rebuild_cmake_project(),
                "build cmake project in </engine/build> directory",
            );
        }
        AppCommand::Rebuild => {
            run_mission(
                || rebuild_cmake_project(),
                "build cmake project in </engine/build> directory",
            );
        }
        AppCommand::Dependencies { cover_old_files } => {
            run_mission(
                || collect_dependencies(cover_old_files),
                "collect dependencies",
            );
        }
    }
}
