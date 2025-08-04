use clap::Parser;
use crate::dependency::collect_dependencies;

pub mod dependency;

#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct Args {
    /// If remove old dependencies' folders and re-download.
    #[arg(short, long, default_value_t = false)]
    cover_old_files: bool
}

pub fn run(){
    let args = Args::parse();
    collect_dependencies(args.cover_old_files).unwrap();
}