use std::{
    env, fs,
    io::{BufRead, BufReader},
    process::{Command, Stdio},
    thread::spawn,
};

use crate::dependency::collect_dependencies;
use clap::{Parser, Subcommand};

pub mod dependency;

#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct AppArgs {
    #[command(subcommand)]
    command: AppCommand,
}

#[derive(Subcommand, Debug)]
enum AppCommand {
    Setup {
        /// If remove old dependencies' folders and re-download.
        #[arg(short, long, default_value_t = false)]
        cover_old_files: bool,
    },
}

async fn async_run_output_command(command: &mut Command) -> anyhow::Result<()> {
    let mut child = command
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn()?;

    let stdout = child.stdout.take().unwrap();
    let stderr = child.stderr.take().unwrap();

    let stdout_task = spawn(async move || {
        let mut reader = BufReader::new(stdout).lines();
        while let Some(Ok(line)) = reader.next() {
            println!("{}", &line);
        }
    });

    let stderr_task = spawn(async move || {
        let mut reader = BufReader::new(stderr).lines();
        while let Some(Ok(line)) = reader.next() {
            println!("{}", &line);
        }
    });

    stdout_task.join().unwrap().await;
    stderr_task.join().unwrap().await;
    let result = child.wait()?;

    Ok(())
}

pub fn run() {
    let args = AppArgs::parse();
    match args.command {
        AppCommand::Setup { cover_old_files } => {
            collect_dependencies(cover_old_files).unwrap();

            let build_dir_path = env::current_dir().unwrap().join("engine/cmake-build-debug");
            if !build_dir_path.exists() {
                fs::create_dir_all(&build_dir_path).unwrap();
            }
            println!("{:?}", &build_dir_path);

            let command = Command::new("cmake")
                .current_dir(&build_dir_path)
                .args(["..", "-G", "Ninja"].into_iter())
                .stdout(Stdio::piped())
                .stderr(Stdio::piped())
                .spawn()
                .unwrap();
        }
    }
}
