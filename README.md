# FastCG

<img src="https://github.com/pboechat/FastCG/blob/master/resources/logo.png" alt="FastCG" height="256px"></img>

A C++ Framework for Rapid 3D Application Prototyping.

Originally developed during my M.Sc. realtime rendering course.

Features:

- Platforms:
    - Windows
    - Linux
    - Android
- Graphics Backends:
    - OpenGL 4.3 (core profile)
    - OpenGL ES 3.2
    - Vulkan 1.1/1.2/1.3
- Materials:
    - Basic shaders like solid color, textured, bump mapping, and more.
- Rendering Paths:
    - Forward
    - Deferred
- Shading Models:
    - Blinn/Phong
- Rendering Techniques:
    - SSAO
    - PCSS
    - HDR/Tonemap
- Scene Loaders:
    - JSON-based format (.scene)
- Image Loaders:
    - JPEG, PNG, TGA, BMP and more (via [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h))
    - DDS ([Microsoft code](https://learn.microsoft.com/en-us/windows/uwp/gaming/complete-code-for-ddstextureloader))
    - KTX (via [KTX-Software](https://github.com/KhronosGroup/KTX-Software))
- Model Loaders:
    - OBJ (via [tinyobjloader-c](https://github.com/syoyo/tinyobjloader-c))
- Immediate-Mode GUI (via [Dear ImGUI](https://github.com/ocornut/imgui))
- Compute Shader Support
- Component-oriented API
