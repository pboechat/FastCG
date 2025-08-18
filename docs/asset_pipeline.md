Asset Pipeline and Resource Management
======================================

Handling assets (shaders, textures, 3D models, etc.) is a crucial part of any 3D graphics framework. FastCG provides tools and conventions to manage assets both at **build time** (aka cook-time) and **runtime**. This documentation covers how FastCG loads assets, what formats are supported, and how the build system can preprocess assets for optimal performance.

Supported Asset Formats
-----------------------

FastCG supports a variety of common asset file formats out-of-the-box:

*   **3D Models:** **OBJ** format for static meshes (via tinyobjloader). OBJ is a simple text format that works well for static geometry. Currently, FastCG does not natively support skeleton animation or more complex formats like FBX or glTF, but those could be converted to OBJ or added via contributions.
    
*   **Images/Textures:** JPEG, PNG, BMP, TGA, and other common image formats are supported for textures (using the stb\_image library to load them at runtime). These are typically used for diffuse maps, normal maps, etc. In addition, FastCG can handle DDS (DirectDraw Surface) format and KTX (Khronos Texture) format, which are container formats often used for GPU-ready textures (with compression like DXT, ETC, ASTC, etc.). By supporting DDS/KTX, FastCG allows you to use pre-compressed textures that load faster and consume less memory on the GPU.
    
*   **Shaders:** GLSL source files (.vert, .frag, etc.) are used for defining shaders. These are not loaded like typical assets at runtime; instead, they are processed by the build pipeline. (See the **Shader Compilation** section below.)
    
*   **Scenes:** JSON .scene files describe a collection of game objects and their components (discussed in the Component System doc). This is a custom format specific to FastCG. It's essentially an asset that gets loaded to construct a world. Scenes allow level data to be authored externally.
    
*   **Other resources:** Any additional files (e.g., material definition files, script files if you integrate a scripting language, etc.) can be managed via the AssetSystem as well.
    

At runtime, the **AssetSystem** is responsible for locating and opening asset files. It maintains a list of "bundle” directories (search paths). By default, FastCG will add the application's own assets/ directory to this search list, as well as any framework default assets. When you request an asset by a relative path (for example, "textures/wood.png"), the AssetSystem will search each known bundle directory for that file. This allows flexibility in where assets are stored (useful in development and when packaging the game). On Android, one of the bundle roots might be inside the APK's assets, whereas on desktop it might be a folder on disk.

Build-Time Asset Processing (aka Asset Cooking)
-----------------------------------------------

FastCG includes a CMake-based asset pipeline that **cooks assets during the build** to ensure they are in an optimal format for runtime. This pipeline is defined in the CMake scripts (see cmake/fastcg\_asset\_cooker.cmake and related calls in fastcg\_targets.cmake). There are a few key tasks it performs:

### Shader Compilation

As mentioned in the Graphics Systems doc, all shader source files in assets/shaders/ are automatically processed. The build will:

*   Copy all GLSL **include files** (common .glsl headers that might be #included in other shaders) to the target asset directory.
    
*   Compile GLSL shader programs (.vert, .frag, .comp, etc.) into the appropriate format:
    
    *   If using **Vulkan**, each shader is compiled to SPIR-V binary. For example, myshader.vert might produce a myshader.vert.spv (or it may keep the same name but contain binary data). The build uses **glslangValidator** (from the Vulkan SDK) to do this compilation with --target-env vulkan1.1 (or higher) flag.
        
    *   If using **OpenGL**, the shaders remain as text (GLSL source). The build might still run them through glslangValidator with --target-env opengl to validate them, but ultimately it will output text files. If the option FASTCG\_USE\_TEXT\_SHADERS is ON, the build simply copies the shader files as-is into the output assets. If it's OFF (default), it might still compile them to SPIR-V if OpenGL can accept SPIR-V (OpenGL 4.6+ with GL\_ARB\_gl\_spirv extension) - however, since FastCG targets 4.3, it likely uses text shaders for OpenGL. Essentially, OpenGL shaders are loaded and compiled at runtime from the text files that are shipped.
        
*   Place the resulting shader files in the build output's assets//shaders/ directory. This way, when you run the program, the shaders it needs are readily available (as either SPIR-V or GLSL text, depending on backend).
    

**What this means for you:** You should write your shader code in the examples/assets/shaders/ folder (or your own project's asset folder if using FastCG as a library). Use .vert for vertex shaders, .frag for fragment shaders, etc. If you have common definitions, put them in .glsl include files and include them in the shader files. When you build, check the build logs for any shader compilation errors (CMake will report them as part of the custom build steps). Fix those, and once built, your shaders will be loaded by FastCG at runtime without the need for runtime compilation delays.

### Texture Compression and Conversion

FastCG's asset cooker can automatically convert images to more efficient formats if you provide instructions (called **recipes**). This is an advanced feature primarily useful if you want to ship optimized textures (like pre-compressed .ktx files with mipmaps) rather than raw PNGs or JPEGs.

The way it works:

*   You create a **recipe file** (with extension .recipe) describing how to cook a particular asset. For example, you might have skybox.recipe that lists six skybox faces and specifies generating a cubemap KTX file with mipmaps.
    
*   During the build, CMake looks for any .recipe files in your assets. For each recipe, it invokes the fastcg\_asset\_cooker.cmake script in a subprocess. That CMake script uses tools like the **KTX-Software** CLI (ktx) for KTX conversion or Microsoft's **texconv** for DDS.
    
*   If the tools are available on your system (ensure they're in PATH or specify their paths), the script will produce the "cooked" asset (e.g., a .ktx or .dds file) as specified.
    

A recipe file is essentially a text file with key=value pairs, one per line. For example, to create a KTX cubemap from 6 images, a recipe might contain (in pseudo-code):

```cmake
source=resources/textures/skyboxes/sky_and_sea/right.png,resources/textures/skyboxes/sky_and_sea/left.png,resources/textures/skyboxes/sky_and_sea/top.png,resources/textures/skyboxes/sky_and_sea/bottom.png,resources/textures/skyboxes/sky_and_sea/front.png,resources/textures/skyboxes/sky_and_sea/back.png
format=R8G8B8A8_UNORM
cubemap=1
```

A recipe requires you to specify source files and properties specific to the recipe type. For example, a texture recipe might have format (such as a compression format or whether to use sRGB), whether to generate mipmaps, etc. At build time, the build system will produce the cooked asset directly into the deploy directory structure, which usually can then be loaded at runtime much faster than the source.

If no recipe is provided for a texture, FastCG will simply copy the image file to the deploy directory structure as-is. You can always choose to manually compress images offline and just include the .ktx or .dds files in your assets if that's easier for you.

### Asset Copying

For all other assets that are not explicitly cooked or compiled, the build system will copy them to the deploy directory structure. This includes things like model files (.obj), JSON scene files (.scene), and any images that remain in standard formats. The CMake scripts have a section that globs all files in the assets directory inside the application source structure and copies them over to the deploy directory structure. Assets that are cooked are excluded from this process to avoid duplicates or copying source GLSL files when SPIR-V binaries have already been compiled.

The assets folder within the deploy structure (ie, deploy/<platform>/<graphics-system>/<application>/assets) will mirror the structure of the assets folder within the source structure (ie, <application>/assets). Here, `<platform>` refers to the target operating system (such as `Windows`, `Linux`, or `Android`), `<graphics-system>` specifies the graphics API in use (like `OpenGL` or `Vulkan`), and `<application>` is the name of your application or example project. At runtime, the AssetSystem is configured to look in these folders.

Using AssetSystem at Runtime
----------------------------

When your application is running, you typically load assets through framework APIs:

*   auto texture = Texture::Load("textures/wood.ktx");Under the hood, Texture::Load will ask AssetSystem to resolve the file path "textures/wood.ktx". The AssetSystem checks each registered bundle directory for a file with that path. In this case, it finds it in the application's assets (deployed by the build). Then it uses the appropriate loader (KTX loader, or stb\_image if it were a PNG) to read the file and create a Texture object. The details of GPU upload are handled by the GraphicsSystem (creating an OpenGL texture or a Vulkan texture).
    
*   auto mesh = Mesh::LoadOBJ("meshes/scene.obj");FastCG's OBJ loader will read the file via AssetSystem (so you don't need to provide a full path, just relative within assets), parse it, and create a Mesh object (which includes vertex buffers, index buffers, etc., created via the Graphics module).
    
*   GameObjectLoader::Load("scenes/level1.scene");The loader will open the JSON, create objects and components as described. This is a high-level one-call way to set up a bunch of objects.
    

The AssetSystem supports adding multiple roots. If you wanted, you could package some assets in a zip or a different folder and add that directory to AssetSystem so it checks there as well. By default, when using the provided CMake structure, the search roots include:

1.  The framework's own FastCG/assets or FastCG/resources (for any built-in assets like default textures or shaders).
    
2.  The application's assets directory (for example, examples/deferred\_rendering/assets/ content copied to build folder).
    
3.  On Android, possibly the internal app path or Android's Asset Manager (handled inside AndroidApplication).
    

Best Practices for Asset Handling
---------------------------------

*   **Keep assets organized:** Use subfolders like textures/, models/, shaders/, scenes/ to group asset types. This not only keeps things tidy but also makes it easier to write asset loading code (as you know where to find things).
    
*   **Use optimized formats in production:** During development, it might be fine to use PNGs and OBJs, but for release, consider using DDS/KTX for textures (to avoid runtime compression) and binary formats for models if file size or loading time becomes a concern. FastCG's pipeline can help with textures via recipes. For models, since only OBJ is built-in, you might pre-convert them to an efficient binary format if needed (though that requires writing a custom loader or extending FastCG).
    
*   **Leverage scene files:** Instead of procedurally creating every object via code, define levels or object prefabs in JSON. This makes iteration faster (you can tweak a JSON and reload rather than recompile C++). FastCG's scene loader can instantiate a whole scene, which is great for testing or content creation pipelines.

The asset pipeline of FastCG strikes a balance between automation and flexibility: common tasks like shader compilation and texture compression can be automated, but you're never prevented from just loading a file directly if you need to. By understanding how assets flow from source files to in-framework objects, you can better utilize the framework and also identify areas to improve or customize for your project's needs.