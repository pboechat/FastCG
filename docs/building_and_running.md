Building & Running
==================

FastCG is designed to run on multiple platforms. Its build system (based on CMake) makes it straightforward to compile the framework and user applications on each supported OS. This guide covers setup, building, and running FastCG-based applications.

Prerequisites
-------------

*   A C++20-compatible compiler is required:
    *   **MSVC:** Visual Studio 2019 or later.
    *   **GCC:** Version 10.0 or later.
    *   **Clang:** Version 11.0 or later.
    *   **Android:** Android NDK r21 or later (includes Clang 11).
    
*   CMake (minimum version 3.20, although newer is recommended). CMake will generate platform-specific project files or Makefiles.
    
*   **Windows:** Microsoft Visual Studio toolchain.
    
*   **Linux:** GCC or Clang toolchains. Ensure you have development packages for X11 (for windowing) if not already present.
    
*   **Android:** Android NDK (r21 or later). You will also need Android SDK if you plan to run on a device/emulator, and CMake should be aware of the NDK toolchain.
    

**Third-party dependencies:** FastCG relies on some libraries (like [stb_image](https://github.com/nothings/stb), [tinyobjloader](https://github.com/syoyo/tinyobjloader), [Dear ImGui](https://github.com/ocornut/imgui), etc.), which are automatically fetched via CMake. For Vulkan support, you should have the Vulkan SDK installed (to get libraries and glslangValidator compiler). If Vulkan SDK is not installed and you choose Vulkan backend, ensure to set VULKAN\_SDK environment or provide paths so CMake can find Vulkan.

### CMake Presets

To configure a build, run CMake from the repository root with one of the existing presets. For example, on a desktop:

```
# On Linux
mkdir build && cd build
cmake --preset linux-gcc-opengl-debug
# On Windows
cmake --preset windows-msvc-opengl-debug
```

| Preset Name                         | Platform | Compiler/Generator           | Build Type      | Graphics System | Description                                                                          |
|-------------------------------------|----------|------------------------------|-----------------|-----------------|--------------------------------------------------------------------------------------|
| linux-gcc-opengl-debug              | Linux    | Unix Makefiles (GCC)         | Debug           | OpenGL          | Build FastCG on Linux using GCC and OpenGL in Debug mode                             |
| linux-gcc-opengl-release            | Linux    | Unix Makefiles (GCC)         | Release         | OpenGL          | Build FastCG on Linux using GCC and OpenGL in Release mode                           |
| linux-gcc-vulkan-debug              | Linux    | Unix Makefiles (GCC)         | Debug           | Vulkan          | Build FastCG on Linux using GCC and Vulkan in Debug mode                             |
| linux-gcc-vulkan-release            | Linux    | Unix Makefiles (GCC)         | Release         | Vulkan          | Build FastCG on Linux using GCC and Vulkan in Release mode                           |
| linux-clang-opengl-relwithdebinfo   | Linux    | Unix Makefiles (Clang)       | RelWithDebInfo  | OpenGL          | Build FastCG on Linux using Clang and OpenGL in RelWithDebInfo mode                  |
| linux-clang-vulkan-relwithdebinfo   | Linux    | Unix Makefiles (Clang)       | RelWithDebInfo  | Vulkan          | Build FastCG on Linux using Clang and Vulkan in RelWithDebInfo mode                  |
| windows-msvc-opengl-debug           | Windows  | Visual Studio 16 2019        | Debug           | OpenGL          | Build FastCG on Windows using MSVC and OpenGL in Debug mode                          |
| windows-msvc-opengl-release         | Windows  | Visual Studio 16 2019        | Release         | OpenGL          | Build FastCG on Windows using MSVC and OpenGL in Release mode                        |
| windows-msvc-vulkan-debug           | Windows  | Visual Studio 16 2019        | Debug           | Vulkan          | Build FastCG on Windows using MSVC and Vulkan in Debug mode                          |
| windows-msvc-vulkan-release         | Windows  | Visual Studio 16 2019        | Release         | Vulkan          | Build FastCG on Windows using MSVC and Vulkan in Release mode                        |
| windows-msvc-opengl-relwithdebinfo  | Windows  | Visual Studio 16 2019        | RelWithDebInfo  | OpenGL          | Build FastCG on Windows using MSVC and OpenGL in RelWithDebInfo mode                 |
| windows-msvc-vulkan-relwithdebinfo  | Windows  | Visual Studio 16 2019        | RelWithDebInfo  | Vulkan          | Build FastCG on Windows using MSVC and Vulkan in RelWithDebInfo mode                 |
| android-opengl-debug                | Android  | Android NDK (Clang)          | Debug           | OpenGL ES 3.2   | Build FastCG for Android using the NDK with OpenGL ES in Debug mode                  |
| android-opengl-release              | Android  | Android NDK (Clang)          | Release         | OpenGL ES 3.2   | Build FastCG for Android using the NDK with OpenGL ES in Release mode                |
| android-vulkan-debug                | Android  | Android NDK (Clang)          | Debug           | Vulkan          | Build FastCG for Android using the NDK with Vulkan in Debug mode                     |
| android-vulkan-release              | Android  | Android NDK (Clang)          | Release         | Vulkan          | Build FastCG for Android using the NDK with Vulkan in Release mode                   |
| android-opengl-relwithdebinfo       | Android  | Android NDK (Clang)          | RelWithDebInfo  | OpenGL ES 3.2   | Build FastCG for Android using the NDK with OpenGL ES in RelWithDebInfo mode         |
| android-vulkan-relwithdebinfo       | Android  | Android NDK (Clang)          | RelWithDebInfo  | Vulkan          | Build FastCG for Android using the NDK with Vulkan in RelWithDebInfo mode            |

After configuration, build the project, again, using one of the existing presets. The build presets are named exactly the same as the configuration presets prefixed with _build-_, e.g., `build-android-vulkan-release`.
    
*   CMake will produce the binaries and also process ("cook") assets for the framework library and any FastCG targets-this means converting, preparing, and copying necessary resource files to the deploy directory.
    

**Note:** The build scripts will compile shader files and process assets automatically (see [Asset Pipeline](./asset_pipeline.md)). For example, if an example's assets include GLSL shader source, the build will run glslangValidator to compile them. Ensure that the **Vulkan SDK's bin directory (for glslangValidator)** is in your PATH or specify FASTCG\_GLSLANGVALIDATOR CMake variable to point to it.

### Platform-Specific Build Notes

*   **Windows:** The framework uses the Win32 API for window creation. No special dependencies are needed beyond what Windows itself provides. Make sure to build in 64-bit mode if using Vulkan (as Vulkan SDK on Windows links to 64-bit libs).
    
*   **Linux:** X11 is used for windowing (via Xlib). It requires X11 development packages (libX11-devel, etc.) installed. Also ensure you have OpenGL drivers and libraries. On Linux, OpenGL context creation is done via GLX by default. For Vulkan, the Vulkan loader (libvulkan.so) should be present (install via your package manager, e.g., vulkan-loader and GPU-specific drivers).
    
*   **Android:** FastCG relies on the native application glue to interface with Android. It also relies on the surface set up by the activity manager, and hooks it with the currently selected graphics system.


Launching
---------

On desktop (Windows/Linux), launching an application is straightforward: you can typically double-click the built executable in your file manager or run it from a terminal (e.g., `./YourAppName`). A window should appear displaying the application's output (for example, a 3D scene if running a graphics example). You can then resize the window, interact with it, or close it to exit. 

On Android, launching an application is a bit more elaborate. The build system produces an APK for each executable target. Additionally, it sets up two extra targets for each executable: one for deploying the APK and another for deploying and launching it. As mentioned above, the application will run as a native activity - note that on Android, most application lifecycle events are handled by the `AndroidApplication` class internally, and input is routed via Android's input system.

Platform Differences at Runtime
-------------------------------

FastCG abstracts most platform differences, but there are a few things to be aware of:

*   **File Paths:** On desktop, assets are simply files on disk (the AssetSystem will search in mounted directories). On Android, asset files might be packaged inside the APK. FastCG's AndroidApplication integrates with Android's asset manager or uses internal storage to access files. Use AssetSystem::Resolve("path/to/file") to get a valid path on all platforms - it will handle locating the file in the bundle or APK.
    
*   **Input and UI:** Mouse and keyboard input exist on Windows/Linux. On Android, touchscreen events are translated to mouse events where possible (e.g., touch = left mouse click, drag = mouse move) and there is limited keyboard input unless an external keyboard is used. ImGui on Android will work, but you control it via touch (which can be clunky for certain UI elements).
    
*   **Performance considerations:** Mobile devices (Android) have less GPU power and different constraints. If using the deferred renderer on mobile, consider simpler scenes or fewer lights. Vulkan on mobile can be advantageous for multithreading and reducing CPU overhead, but it requires the device support and more GPU memory for swapchain, etc. The framework's default settings try to be reasonable, but you might tweak quality (texture resolution, SSAO quality, etc.) based on platform.
    

In general, if your goal is cross-platform compatibility, develop and test on both PC and Android to catch any platform-specific issues. FastCG's goal is to minimize those issues by handling them internally.

Troubleshooting Build Issues
----------------------------

*   If CMake can't find a dependency (like Vulkan or an image library), check that all submodules or packages are fetched. The cmake/dependencies.cmake will either use included third-party code or look for system installs. Running git submodule update --init might be necessary if the project uses submodules for dependencies.
    
*   If you get compile errors related to platform headers, ensure the correct FASTCG\_PLATFORM is set. CMake tries to do this for you. For example, if you accidentally try to compile the Android code on Linux without the NDK, you'll get errors - instead, use the provided scripts or toolchain file for Android.
    
*   On Windows, if using MSVC, make sure to select x64, as the framework might not compile for 32-bit out of the box (especially with Vulkan, which expects 64-bit).
    
*   For any runtime errors (like a blank screen or crash), use the console output or logs. FastCG's logging (e.g., FASTCG\_LOG\_INFO macros) will print messages that can help identify missing assets or unsupported features on a given platform.
    

Building and running FastCG should be relatively straightforward thanks to its build system. Once you see the example apps running on your platform, you're ready to start developing your own application using FastCG as a library. In the next sections, we'll discuss how to create game objects, use components, and load assets in your application code.