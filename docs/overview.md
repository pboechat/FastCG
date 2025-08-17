FastCG Overview
===============

FastCG is a C++ framework for rapid prototyping of 3D applications. It provides a foundation to quickly build graphics applications and small games by handling low-level details like rendering, windowing, and asset loading. FastCG was originally developed as part of an M.Sc. real-time rendering course and has since grown into an open-source framework focused on modularity and ease of use. Developers can use FastCG to create demos, small games, or real-time graphics experiments, and are encouraged to both use and contribute to the codebase.

Key Features
------------

*   **Cross-Platform Support:** FastCG runs on Windows, Linux, and Android, with abstractions for window management, filesystem, input, graphics and more. The same code can compile and run on desktop or mobile with no changes!
    
*   **Multiple Graphics Backends:** It supports both OpenGL (Core 4.3 and OpenGL ES 3.2) and Vulkan (1.1/1.2/1.3) graphics systems/backends. This means you can target a wide range of hardware - from older OpenGL-based systems to modern Vulkan-capable devices - using a single API!
    
*   **Modern Rendering Techniques:** The framework includes forward and deferred rendering paths, with built-in support for advanced lighting and post-processing effects (ambient occlusion, soft-shadows, HDR tone mapping, etc.). These features are extensible, allowing developers to quickly experiment with graphics features and add custom effects.
    
*   **Entity-Component System (ECS):** FastCG uses a component-oriented design, which makes it easy to create game objects with mix-and-match behaviors. Instead of deep inheritance hierarchies, you compose functionality by attaching components (like renderable meshes, lights, or custom behaviors) to game objects.
    
*   **Asset Management:** The framework can load common asset formats. Image formats like PNG, JPEG, TGA, BMP (via [stb_image](https://github.com/nothings/stb)) and GPU-friendly formats like DDS and KTX are supported. It also includes a model loader for OBJ files and a JSON-based **.scene** format for describing entire scenes. A built-in **AssetSystem** helps locate and load these resources at runtime.
    
*   **UI and Debug Tools:** FastCG integrates [Dear ImGui](https://github.com/ocornut/imgui) for immediate-mode GUI, so you can create in-app debugging tools or interfaces easily. It also provides logging and a debug menu system to assist in development and profiling. Compute shader support is available for general-purpose GPU computations or effects.
    
In the following sections, we'll dive deeper into FastCG's architecture and systems. You'll learn how the framework is structured, how to build and run it on different platforms, and how to utilize and extend its graphics and component systems. Use this documentation as a starting point for both using FastCG in your project and as a reference for contributing to its development!
