Graphics System
===============

FastCG supports multiple graphics APIs through an abstraction layer. This allows the framework (and your game code) to use a common interface for rendering while targeting different graphics backends. Currently, FastCG implements support for **OpenGL** and **Vulkan**, each with some variations (desktop vs. mobile):

Supported APIs
--------------

*   **OpenGL 4.3 (Core Profile):** Primarily for desktop platforms (Windows/Linux). FastCG's OpenGL backend implements features like compute shaders and a post-processing pipeline while maintaining compatibility with slightly older GPUs.
    
*   **OpenGL ES 3.2:** Used on Android and potentially other mobile platforms. OpenGL ES 3.2 is roughly equivalent to OpenGL 4.3 in terms of shader and pipeline capabilities, but tailored for mobile. FastCG's OpenGL backend will automatically adjust to GLES when running on Android.
    
*   **Vulkan 1.1 (with support for 1.2/1.3):** For cutting-edge graphics and explicit GPU control, FastCG provides a Vulkan backend. Vulkan is supported on both desktop and Android (assuming the device and drivers support it). The framework's Vulkan integration was built to take advantage of advanced Vulkan features and versions (1.2, 1.3) when available, while remaining backward-compatible with 1.1.
    

When building FastCG, you choose one graphics backend to compile in (OpenGL or Vulkan) via CMake presets (see [Building & Running](./building_and_running.md)).

Abstraction Design
------------------

FastCG's Graphics module defines a set of abstract classes (prefix Base\*) that represent common GPU resources and operations:

*   **GraphicsSystem:** This is an abstract base class that serves as the central interface for the graphics API. Users should not instantiate this class directly; instead, FastCG provides concrete implementations (such as OpenGLGraphicsSystem and VulkanGraphicsSystem) that are selected based on the chosen backend. The interface allows users to create and manage graphics objects like buffers, textures, and shaders without needing to know the underlying API.
    
*   **GraphicsContext:** Represents the rendering context or device. It implements a state-driven command interface, akin to the immediate mode in OpenGL or legacy DirectX pipelines, allowing granular control over rendering states.
    
* **Buffer, Texture, Shader & More:** FastCG provides a unified abstraction for graphics objects. Base classes like BaseTexture and BaseShader define a consistent interface and share common logic, while API-specific implementations (such as OpenGLTexture/VulkanTexture and OpenGLShader/VulkanShader) handle backend-specific details. This design abstracts low-level operations—like OpenGL buffer bindings or Vulkan descriptor set updates—so you can focus on high-level rendering without worrying about the underlying API complexities.
    
*   **Rendering Workflow:** In OpenGL, rendering involves binding VAOs, textures, and issuing draw calls. In Vulkan, it involves recording command buffers, managing render passes, and pipelines. FastCG's abstraction tries to unify these by providing higher-level calls. For example, a Renderable component will ultimately call GraphicsSystem::DrawMesh(...) which the underlying system implements appropriately for GL or Vulkan.


Because Vulkan is a more explicit API, the VulkanGraphicsSystem and related classes handle things like pipeline creation (VulkanPipeline class), render pass setup (VulkanRenderPass), and descriptor sets (VulkanDescriptorSet). The OpenGL path doesn't need these explicit objects, so many of those classes are specific to the Vulkan folder. The framework ensures that both backends produce the same visual results even if the code paths differ significantly under the hood.
    

**Note:** FastCG's architecture allows adding new graphics backends in the future. For example, one could introduce a DirectX 12 backend by implementing the same BaseGraphicsSystem interface. If you are interested in contributing a new backend or improving the existing ones, the abstraction layer will help isolate your changes to the Graphics module.


Shaders and Pipeline States
---------------------------

FastCG uses GLSL for shader source code in both OpenGL and Vulkan backends, but handles them differently at build and runtime:

*   In **OpenGL**, shader source files (vertex and fragment shaders, etc.) are loaded as text and compiled at runtime by the OpenGL driver. The framework provides these as GLSL source strings when creating an OpenGL shader program.
    
*   In **Vulkan**, shaders must be in SPIR-V binary format. FastCG's build process will **pre-compile GLSL to SPIR-V** using the Vulkan GLSL compiler. This happens during the CMake build stage (see Asset Pipeline doc), producing .spv or binary shader files that the framework then loads into Vulkan shader modules. At runtime, VulkanGraphicsSystem creates pipeline objects that encapsulate these shader modules along with other state (blend, depth, etc.).
    

To ensure consistency, FastCG's build system use **glslangValidator** to compile shaders. The build system knows which shaders to compile by looking in the project's assets/shaders/ directory. It targets GLSL 4.30 for OpenGL or GLSL ES 3.20 for mobile, and Vulkan SPIR-V for the Vulkan backend. This means as a developer, you can write one set of shader code in GLSL, and the build will handle generating the right format depending on the chosen graphics API.

In summary, FastCG's graphics systems give you flexibility to target different APIs and techniques without changing your game logic. You write your rendering code against the FastCG API (creating materials, components, etc.), and the framework worries about whether that means making OpenGL calls or building Vulkan command buffers. This approach also means you can switch backends simply by rebuilding the framework with a different setting - useful for testing performance or compatibility. If you're developing with contribution in mind, understanding this abstraction will help you extend FastCG's graphics capabilities in a clean way.