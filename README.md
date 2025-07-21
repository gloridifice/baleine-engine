# Baleine Engine 🐋

Hello! This is a game engine project for personal learning powered by C++ and Vulkan.

It's seperated into four part in `src` directory now.

### Project structure

The root folder is rust project for building C++ project. See the section below.

- `src`: rust source folder.

`engine/` folder is the real position for engine's sources.

- `baleine_type`: simple type alias lib of STL for this project's naming convention, like `u32 i32 f32 Vec String Unique Shared`.
- `baleine_vulkan`: vulkan wrapper lib.
- `baleine_render`: most render features implementation using libs above.
- `src`: engine executable file source code.

## Build & Run

This project uses a rust project for automatically task and dependencies management for C++ project.  

1. If you don't install rust toolchain, see [Install Rust](https://www.rust-lang.org/tools/install) here;
2. Clone this repository, then run `cargo run` in terminal. It will download third party libs and generate CMakeLists in `engine/third_party/auto/` directory;
3. Then just open the engine folder by your IDE and run cmake project.

