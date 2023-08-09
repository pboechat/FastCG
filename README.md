# FastCG

**FastCG**: A C++ Framework for Rapid 3D Application Prototyping.

Originally developed during my M.Sc. realtime rendering course.

Features:

- Platform Support:
    - Windows
    - Linux
- Rendering Backend Support:
    - OpenGL 4.3 (core profile)
    - Vulkan 1.3
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
    - Supports OBJ models (via [tinyobjloader-c](https://github.com/syoyo/tinyobjloader-c))
- Immediate-mode GUI support (via [Dear ImGUI](https://github.com/ocornut/imgui))
- API Design:
    - Unity-inspired component-based API.
