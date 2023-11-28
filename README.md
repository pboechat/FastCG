# FastCG

<img src="https://github.com/pboechat/FastCG/blob/master/resources/logo.png" alt="FastCG" height="256px"></img>

A C++ Framework for Rapid 3D Application Prototyping.

Originally developed during my M.Sc. realtime rendering course.

Features:

- Platform Support:
    - Windows
    - Linux
    - Android
- Rendering Backend Support:
    - OpenGL 4.3 (core profile)
    - OpenGL ES 3.2
    - Vulkan 1.1/1.2/1.3
- Material System:
    - Basic shaders including diffuse, specular, bump, and more.
- Rendering Paths:
    - Forward
    - Forward+ (Upcoming)
    - Deferred
- Rendering Techniques:
    - SSAO
    - PCSS
    - HDR/Tonemap (Upcoming)
- Asset Management:
    - Supports various image formats (via [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h))
    - Support to KTX (via [KTX-Software](https://github.com/KhronosGroup/KTX-Software)) and DDS.
    - Supports OBJ models (via [tinyobjloader-c](https://github.com/syoyo/tinyobjloader-c))
- Immediate-mode GUI support (via [Dear ImGUI](https://github.com/ocornut/imgui))
- API Design:
    - Component-oriented API.
