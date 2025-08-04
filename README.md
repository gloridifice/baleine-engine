# Baleine Engine 🐋

Hello! This is a game engine project for personal learning powered by C++ and Vulkan.

## Project structure

The root folder is rust project for building C++ project. See the section below.

- `src`: rust source folder.

`engine/` folder is the real position for engine's sources.

- `engine/baleine_type`: simple type alias lib of STL for this project's naming convention, like `u32 i32 f32 Vec String Unique Shared`;
- `engine/baleine_vulkan`: vulkan wrapper lib;
- `engine/baleine_render`: most render features implementation using libs above;
- `engine/src`: engine executable file source code.

## Build & Run

This project uses a rust project for automatically task and dependencies management for C++ project.  

1. [Install Rust](https://www.rust-lang.org/tools/install) & [Install CMake](https://cmake.org/download/) & [Install Vulkan SDK](https://vulkan.lunarg.com/);
2. Clone this repository, then run `cargo run` in terminal. It will download third party libs and generate CMakeLists in `engine/third_party/auto/` folder;
3. Then just open the `engine/` folder by your IDE and run the cmake project.

## Dependencies

Third party libraries (or dependencies) are defined in `Dependencies.toml`.
It will be read by the rust program.

Fields of a dependency are below:

- `url`: url of a dependency on GitHub;
- `tag` or `branch`: which tag or branch to be downloaded of the dependency's git repository;
- `exclude_from_all`: `bool` define if add `EXCLUDE_FROM_ALL` property for this repository's subdirectory into `CMakeLists.txt`;

## Roadmap

- [ ] Render API