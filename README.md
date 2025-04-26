<h1 id="title" align="center">Soft Cube Engine</h1>

<p align="center"><img src="https://socialify.git.ci/iRevolutionDev/softcube/image?forks=1&issues=1&name=1&owner=1&pattern=Plus&pulls=1&stargazers=1&theme=Auto" alt="marvel-aspnet" width="640" height="320" /></p>

<p align="center">A modern voxel-based game engine inspired by Minecraft, built with C++, BGFX, and SDL3.</p>

## Features

- Modern C++ codebase (C++20/26)
- Cross-platform support through BGFX and SDL3
- Entity Component System using EnTT
- Modular engine architecture separate from game code
- Scene-based game flow
- ImGui-based debugging interface

## Project Structure

The project is organized into two main components:

1. **Engine** - Core functionality and systems
2. **Game** - Game-specific code and assets

For detailed information about the project structure, see [Project Structure Documentation](docs/project_structure.md).

## Building

### Requirements

- CMake 3.29 or higher
- C++20 compatible compiler (Visual Studio 2019+, Clang 10+, GCC 10+)
- Git (for fetching dependencies)

### Build Steps

1. Clone the repository:

```
git clone https://github.com/yourusername/softcube.git
cd softcube
```

2. Configure with CMake:

```
cmake -B build
```

3. Build:

```
cmake --build build
```

## Dependencies

The project uses the following external libraries:

- [BGFX](https://github.com/bkaradzic/bgfx) - Cross-platform rendering library
- [SDL3](https://github.com/libsdl-org/SDL) - Cross-platform window and input handling
- [EnTT](https://github.com/skypjack/entt) - Entity component system
- [ImGui](https://github.com/ocornut/imgui) - Immediate mode GUI
- [FreeType](https://www.freetype.org/) - Font rendering
- [spdlog](https://github.com/gabime/spdlog) - Fast C++ logging library

Dependencies are automatically fetched and built by the CMake build system.

## Architecture

The engine is built around the following core systems:

- **Renderer** - BGFX-based rendering system
- **Window** - SDL3 window management
- **Input** - Input handling and mapping
- **Scene** - Scene management and transitions
- **ECS** - Entity-component-system for game objects

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs or feature requests.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
