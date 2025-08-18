
Rendering System
================


In this section, we introduce FastCG's rendering system - a core component designed to manage how scenes are drawn on screen. The system provides two distinct rendering paths tailored for different lighting and performance needs. Its design is modular, supporting both forward and deferred rendering, ensuring efficient use of modern graphics APIs like OpenGL and Vulkan. Additionally, FastCG employs an advanced material sorting and draw call batching strategy. This strategy groups similar rendering commands together and minimizes costly state changes, which optimizes resource usage and maintains high performance, even in complex scenes.

Rendering Paths
---------------

FastCG implements two rendering paths: **Forward rendering** and **Deferred rendering**. Both are available regardless of the graphics API:

*   **Forward Rendering:** In forward mode, lighting calculations are done in a single pass while drawing each object. This is simpler and often sufficient for scenes with few lights. It's also more straightforward for translucent objects. FastCG's forward renderer supports basic lighting (Blinn-Phong) and shadow mapping.
    
*   **Deferred Rendering:** In deferred mode, the framework first renders the scene geometry into G-buffers (position, normals, albedo, etc.), then does lighting in screen-space as a second pass. FastCG's deferred renderer enables more complex scenes with many lights, and advanced effects like SSAO (which uses the depth and normal buffers) and HDR tone mapping. Features like PCSS soft shadows are integrated here as well.
    

You can choose the rendering path via the **ApplicationSettings** structure. For example, in the constructor of your application class, you can set the rendering path as follows:

```cpp
MyApplication::Application()
    : Application({"my_application", {}, {RenderingPath::DEFERRED}})
{
```

Both paths are implemented on top of the GraphicsSystem, so they work with either OpenGL or Vulkan transparently. Performance may differ (Vulkan can handle many objects and lights more efficiently), but the visual output aims to be the same.

Material Sorting and Draw Call Batching
---------------------------------------

FastCG implements material sorting and draw call batching, which is a performance optimization approach to efficiently manage and execute rendering commands. Its primary goal is to minimize the overhead associated with state changes and draw calls-operations that are often costly for the GPU in terms of performance.

## Key Concepts

- **Batching Draw Calls:**  
    FastCG's strategy is to collect similar rendering commands and groups them together. By processing these grouped commands as a single batch, FastCG can reduce the number of individual draw calls sent to the GPU, helping to improve rendering throughput and overall performance.

- **Optimal Resource Usage:**  
    By minimizing state changes (such as switching shaders or textures), the system conserves valuable computational resources. This approach leverages GPU capabilities better, ensuring that rendering tasks are executed in a streamlined manner.

- **Dynamic Grouping:**  
    The framework dynamically determines the best way to group rendering commands based on object properties and current scene demands. This dynamic nature allows it to adapt to a wide range of rendering scenarios, from simple to highly complex scenes.

## User Benefits

- **Improved Rendering Performance:**  
    By reducing overhead from excessive state changes and draw calls, applications using FastCG can achieve smoother and faster rendering, even under heavy loads.

- **Enhanced Scalability:**  
    The system's ability to batch similar commands makes it well-suited for complex scenes where many objects need to be rendered simultaneously, ensuring a scalable solution across various hardware configurations.

- **Simplified Debugging and Optimization:**  
    The clear separation of rendering batches helps developers identify bottlenecks or inefficiencies in the rendering process, offering a more manageable approach to optimization.


