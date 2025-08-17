Project Structure
=================

FastCG's source code is organized into a clear module structure, making it easy to locate functionality. Understanding the project layout will help you navigate the codebase and identify where to implement new features or fix issues. At a high level, the repository consists of **FastCG**'s code, example projects, and build configuration files.

Repository Layout
-----------------

```
FastCG/                 # Framework source code (library)  
├── include/FastCG/     # Public header files, organized by module  
├── src/                # Source files for each module (mirrors include/ structure)  
├── assets/             # (Optional) Framework-level asset files (textures, etc.)  
├── resources/          # Additional resources (e.g., default textures, generated config)  
└── ... (CMake files, scripts, etc.)  
examples/               # Example projects demonstrating framework usage  
├── deferred_rendering/   
├── bump_mapping/  
├── ssao/  
├── ... (other examples)  
└── each example has its own src/ and assets/ subdirectories  
cmake/                  # CMake build scripts (toolchains, dependency finders, asset pipeline)  
scripts/                # Utility scripts (formatting, cross-compiling helpers)   
```

### The Framework

The **FastCG/** directory contains the framework code, divided into modules corresponding to different systems:

*   **Core:** Low-level utilities and platform-independent helpers. This includes encoding, hashing, collection and string utilities, logging, etc. Core defines basic data structures and helper functions used throughout the framework.
    
*   **Platform:** Platform-specific implementations and abstractions. FastCG abstracts the application entry point and windowing system here. There's a base application class (BaseApplication) and subclasses for Windows (WindowsApplication using Win32), Linux (X11Application using X11), and Android (AndroidApplication). The Platform module also provides file I/O (FileReader/FileWriter) and timing (Timer) utilities. The idea is that the rest of the framework can be platform-agnostic by relying on the Platform layer for OS interactions.
    
*   **Graphics**: This module provides a common point to creating textures, shaders, and making draw calls, without depending on specific graphics APIs. As a starting point, please take a look at the BaseGraphicsSystem abstract class with is implemented for each API.
    
    *   _OpenGL:_ Located in Graphics/OpenGL/ (e.g., OpenGLGraphicsSystem, OpenGLBuffer, etc.), targeting OpenGL 4.3 (or OpenGL ES 3.2 on mobile).
        
    *   _Vulkan:_ Located in Graphics/Vulkan/ (e.g., VulkanGraphicsSystem, VulkanBuffer, VulkanPipeline), targeting Vulkan 1.1+.
        
*   **Rendering:** High-level rendering system built on top of the Graphics module. This includes:
    
    *   **Camera:** Camera component for view/projection management.
        
    *   **Lights:** Light components (point lights, directional lights) and lighting models.
        
    *   **Materials and Shaders:** The material system defines materials and associates them with shader programs. Material definitions (possibly via JSON or code) specify shader asset files and parameters. FastCG supports standard lighting models like Blinn-Phong for now. Compute shader support is also available for special effects.
        
    *   **Mesh and Renderable:** The mesh loader (e.g., OBJLoader) and the Renderable component allow you to attach 3D geometry to a game object for rendering. The Rendering module handles grouping renderables, submitting draw calls, and applying rendering techniques (forward or deferred). It also includes features like shadow mapping (PCSS) and screen-space effects (SSAO, HDR tone mapping) built into the deferred renderer.
        
    *   **World Renderers:** Classes like ForwardWorldRenderer and DeferredWorldRenderer implement the specifics of forward vs. deferred rendering paths. They work in conjunction with the World module to traverse visible objects and render them with the chosen technique.
        
*   **World (Entity-Component System):** Manages game objects and components. The World module defines GameObject (which represents an entity in the scene) and the base Component class, along with various concrete component types (found in other modules, e.g., Renderable in Rendering, Light in Rendering, or user-defined behaviors). It also includes a WorldSystem that updates all game objects and components each frame, and utilities for loading/saving game objects (e.g., GameObjectLoader for .scene files).
    
*   **Assets:** Handles asset loading and resource management. The AssetSystem can mount asset directories and resolve file paths. It uses third-party libraries for loading images (via [stb_image](https://github.com/nothings/stb) for formats like PNG, JPEG, etc.) and models (via [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) for OBJ files). This module also defines an **Asset Pipeline** integrated with the build system (discussed in the [Asset Pipeline](./asset_pipeline.md) section) that can preprocess assets (like compiling shaders or compressing textures).
    
*   **Input:** Abstraction for input devices. It provides key code definitions and input event handling (button and key presses, etc.). The Application base class uses this to forward input events (OnKeyPress, OnMouseMove, etc.) to user code.
    
*   **UI:** Integration with [Dear ImGui](https://github.com/ocornut/imgui) for creating graphical interfaces. This module can initialize ImGui and render UI elements. It's useful for developer tools (like showing debugging info or tweaking variables in real-time).
    
*   **Debug:** Contains tools for development and debugging. For example, DebugMenuSystem allows a developer to toggle framework settings or display diagnostics via an in-game menu (often implemented with ImGui). Logging is also part of Debug/Core (print to console or on-screen).
    

Each module is relatively self-contained, communicating with others through well-defined interfaces. For instance, the Rendering module queries the World for objects with Renderable components and uses the Graphics module to draw them. This modular design means you can work on or replace one part (say, add a new Graphics backend, or integrate a physics engine as a new module) without heavily affecting other parts.

Examples
--------

The **examples/** directory contains sample applications that demonstrate how to use FastCG. Each example is a small program (with its own main or equivalent via the FASTCG\_MAIN macro) that sets up a scene and showcases certain features:

*   **deferred\_rendering:** Demonstrates the deferred rendering pipeline with multiple lights, SSAO, shadows, etc.
    
*   **bump\_mapping:** Shows how to use normal mapping in materials.
    
*   **ssao:** Focused example of screen-space ambient occlusion.
    
*   **pcss:** Example of percentage-closer soft shadows (shadow filtering technique).
    
*   **matrix\_multiply:** A simple compute shader example, performing shader-based GPU matrix multiplication (matmul) to showcase the compute shader support.
    
Each example has its own assets/ folder for scene data, models, and shaders specific to that demo. The examples are great starting points to see how an application is structured and how to use the engine's API to load a scene, manage objects, and run the game loop. New contributors can also use them to test changes to the engine.

By understanding this project structure, you'll know where to find the code relevant to your area of interest. Whether you want to improve rendering, add a new component type, or fix a platform-specific bug, the modular directory layout should guide you to the right place.
