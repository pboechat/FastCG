# FastCG

**FastCG** is a C++ application framework for fast prototyping of 3D applications.

Started in the *realtime rendering* course taken during my M.Sc.

It features:

 - Support to multiple platforms:
    - Windows
    - Linux
 - Support to multiple rendering backends:
    - OpenGL 4.3 (core profile)
    - Vulkan 1.3 (TODO)
 - Basic material system: diffuse, specular, bump, etc.
 - Multiple rendering paths: 
    - Forward 
    - Forward+ (TODO)
    - Deferred
 - Multiple rendering techniques:
    - SSAO
    - PCSS
    - HDR/Tonemap (TODO)
 - Simple asset management:
    - Support to multiple image types (via [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h))
    - Support to OBJ models (via [tinyobjloader-c](https://github.com/syoyo/tinyobjloader-c))
 - Support to immediate-mode GUI (via [Dear ImGUI](https://github.com/ocornut/imgui))
 - Unity-like component-based API