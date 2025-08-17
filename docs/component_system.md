World and Component System
==========================

FastCG employs an **Entity-Component System (ECS)**\-like architecture to manage game objects and their behavior. Instead of rigid class hierarchies, it uses flexible components that can be attached to game objects (entities) to give them particular properties or functionality. This section explains how the world system is structured, how to use components, and how to extend the system with custom components or behaviors.

Game Objects and Components
---------------------------

A **GameObject** in FastCG represents an entity in the world - it can be anything from a player character, a light source, to an invisible trigger zone. Every GameObject has a few basic things:

*   A **Name** (for identification/debugging).
    
*   A **Transform** (position, rotation, scale in the world).
    
*   A collection of **Components** attached to it.
    

The **Transform** is a fundamental component that every GameObject has by default. It defines the object's local position, rotation, and scale, and also maintains parent-child relationships. GameObjects can be attached to one another forming a scene graph; if you set GameObject B's parent to GameObject A, B's transform becomes relative to A's transform.

Beyond the Transform, all other functionality is added via **Component** objects. A component might represent renderable geometry, a light, a camera, a custom behavior, or any other feature. Components do not typically know about each other directly; they communicate through the GameObject or via systems. This decoupling makes it easy to mix components as needed.

FastCG provides a base class Component (which itself inherits from a small reflection interface Inspectable). Each specific component type inherits from Component. Under the hood, components are identified by a **ComponentType** (which includes a name string and a pointer to a base type for inheritance info). The framework uses this to identify components, especially when loading from scene files or when querying objects (e.g., “get me the Light component attached to this object”).

Key points about components:

*   A GameObject can have multiple components of different types, but usually one of each type (e.g., one Renderable, one Camera, etc. - there's typically no reason to attach two of the same kind like two Camera components).
    
*   You can retrieve a component by type from a GameObject. For example, gameObject->GetComponent() would return the Renderable if present.
    
*   Components can depend on each other indirectly. For instance, the Renderable component will use the GameObject's Transform when rendering the object, and a Camera component might also use its object's Transform to determine the view origin.
    

Built-in Components
-------------------

FastCG comes with a set of built-in component classes to cover common needs:

*   **Renderable:** Makes a GameObject render a mesh. A Renderable holds data like a reference to a Mesh (geometry), a Material (which in turn references shaders and textures), and perhaps other rendering parameters. If a GameObject has a Renderable, the framework's rendering system will include it in the draw calls. Typically you attach a Renderable and a Transform to any visible object.
    
*   **Camera:** Represents a viewpoint for rendering. A Camera component has properties for field of view, near/far clipping planes, and can produce a view-projection matrix. One camera (often tagged as the “Main Camera”) will be used by the renderer to render the scene from that perspective. GameObjects with cameras can be moved or rotated by adjusting their Transform.
    
*   **Light:** FastCG supports different light components. There is a base Light class (which might derive from Component) and specific types like **DirectionalLight** and **PointLight** (and possibly SpotLight if added). Attaching a Light to a GameObject will cause the framework's lighting system to consider that light in the scene. Directional lights assume the GameObject's orientation for the light direction (position is irrelevant or considered infinite), while point lights use the object's position (and maybe a radius).
    
*   **Behaviour (Custom Components):** A **Behaviour** is an abstract component meant to be subclassed to create custom behaviors. It's like a "script" attached to an object. For example, you might create a SpinningBehaviour that rotates an object over time, or a PlayerController that moves a character based on input. To create a new behavior, you subclass FastCG::Behaviour and implement the Update(float deltaTime) method (and optionally overrides like OnStart, OnEnd). Behaviours are updated every frame by the framework's **WorldSystem**.
    
*   **Others:** FastCG provides many additional components such as Fog (to define fog parameters in the scene) and utility components like camera controllers. Feel free to explore the codebase to find more about them!

Under the hood, each component type has a static TYPE member used for identification. FastCG's reflection-lite system means you can query a component's type name at runtime (useful for debugging or for the editor/loader to instantiate components by name).

World System and Update Cycle
-----------------------------

The **WorldSystem** is the part of the framework that manages all game objects and global scene state. It keeps track of the list of all active GameObjects in the scene. Each frame, the WorldSystem (often in conjunction with the Application class) will:

1.  **Update Transforms:** Propagate any changes in object transforms through the hierarchy (so if a parent moved, children's world positions update).
    
2.  **Process Behaviors:** Call the Update(deltaTime) on all Behaviour components attached to active GameObjects. This is where your game logic runs each frame. For example, the LightsAnimator in the deferred\_rendering example likely updates light positions/intensities here each frame.
    
3.  **Trigger Events:** The WorldSystem may also handle other per-frame tasks like removing destroyed objects, checking collisions if a physics system is present (currently, physics is not built-in), or other world interactions.
    
4.  **Call Rendering:** After updating the world state via components and behaviors, the WorldSystem hands over to the Rendering system (via a Renderer or RenderingSystem) which gathers all Renderable components and draws them using the active Camera, etc.
    

The update loop is orchestrated by the Application (which calls WorldSystem and RenderingSystem in the right order), but as a user of FastCG you typically don't have to manage that manually - you just implement your component behaviors and let the framework call them.

The Application also routes input events to GameObjects or components. For example, if you override OnKeyPress in your Application or a specific component listening for input, you can then respond to user inputs each frame.

Creating and Destroying Objects
-------------------------------

To create a new GameObject at runtime, you can use the static factory method provided:

```cpp   
GameObject* obj = FastCG::GameObject::Instantiate("ObjectName",                         
/*scale=*/glm::vec3(1.0f),                         
/*rotation=*/glm::quat(1,0,0,0),                         
/*position=*/glm::vec3(x, y, z));
```

This will allocate a new GameObject (with a new Transform). You can then attach components to it:

```cpp   
auto renderable = new FastCG::Renderable(meshPtr, materialPtr);  
obj->AddComponent(renderable);
```

FastCG's memory management for components uses raw pointers (with some internal unique\_ptr usage for certain types). When you AddComponent, the WorldSystem takes ownership of it. To remove an object, call FastCG::GameObject::Destroy(obj), which will schedule it for removal (the actual deletion might happen at end of the frame to avoid corrupting the update loop).

Use the provided Instantiate and Destroy methods; do not manually delete GameObjects or components, as the framework manages their lifetimes.

Scene Loading (JSON .scene)
---------------------------

FastCG supports loading a whole scene from a file, so you can design a level or setup in JSON and avoid hardcoding object creation. The **scene file (.scene)** is a JSON format that describes game objects, their transforms, and attached components with properties.

For example, a simplified snippet of a scene file might look like:

```json
{    
    "GameObjects": [      
        {        
            "name": "Camera1",        
            "position": [0, 5, -10],        
            "components": [          
                { "type": "Camera", "fov": 60.0 }        
            ]      
        },      
        {        
            "name": "Light1",        
            "position": [0, 4, 0],        
            "components": [          
                { "type": "PointLight", "color": [1,1,1], "intensity": 2.0 }        
            ]      
        },      
        {        
            "name": "Actor",        
            "position": [0,0,0],        
            "components": [          
                { "type": "Renderable", "mesh": "Meshes/actor.obj", "material": "Materials/actor.mat" }      
            ]      
        }    
    ]  
}
```

_(Note: The exact schema might differ, but this illustrates the idea.)_

When you call GameObjectLoader::Load("path/to/scene.scene"), FastCG will parse the JSON, create each GameObject, set its Transform, and then create each listed component. It uses the component type field in the JSON to look up a component class. The framework has a **ComponentRegistry** that maps type names (like "Renderable", "PointLight", etc.) to factory functions. This is how it knows to create a FastCG::Renderable object when it sees type "Renderable". If you create custom components and want them to be loadable from scenes, you would register them with a name in this registry.

The scene loader also handles linking references, like loading the mesh file and material in the above example (the AssetSystem would be used to load "Meshes/actor.obj", for instance). It effectively automates what you could do manually in C++ - so it's a useful way to populate a complex scene.

Custom Components and Extensibility
-----------------------------------

One of the advantages of an ECS is the ease of adding new component types. In FastCG, to create a new component, you would:

1.  Subclass FastCG::Component or one of its derivatives. Often, for logic components, you subclass FastCG::Behaviour (which is itself a Component) so you get the per-frame Update callback.
    
2.  If your component needs to be recognized by the scene loader or other systems, give it a unique TYPE. There are macros like FASTCG\_DECLARE\_COMPONENT or similar (for example, in the code you might see FASTCG\_DECLARE\_COMPONENT(DerivedType, BaseType) in the class definition) which set up the static TYPE member. Use these to define the type name and hierarchy.
    
3.  Implement whatever data and methods you need. For a pure logic component, it might just override OnStart() or Update(). For something that interacts with rendering, you might tie into the RenderingSystem (or simply attach it alongside other components).
    
4.  Register the component if needed. The framework might automatically register components with the static TYPE, but if not, you can add an entry to the ComponentRegistry. (Check FastCG's documentation or code for how built-in components are registered. Often this is done in a centralized place by instantiating a dummy object of each type or via static initializers.)
    

Once that's done, you can attach your new component to any GameObject at runtime. If you want to instantiate it from a scene file, ensure the scene file uses the type string you set. For example, if you created a component Spinner (to spin objects), and you set its TYPE name to "Spinner", then in JSON you'd add { "type": "Spinner", "speed": 90 } in a game object's component list, and the loader will call your component's constructor (likely passing the JSON properties to it, possibly via an Inspectable interface).

**Lifecycle hooks:** Components can implement OnStart and OnEnd if they inherit Behaviour, which are called when the object is activated or about to be destroyed, respectively. Use these for initialization and cleanup. Also, since components can have a hierarchy (a component type can inherit from another component type), make sure to call base class methods if you override them and the base has important behavior.

Tying it All Together
---------------------

To summarize, the component system in FastCG is what makes the framework flexible and extensible:

*   The **WorldSystem** keeps track of all objects and updates them.
    
*   GameObjects are containers for state (Transform) and Components.
    
*   Components define what an object _is_ or _does_ (renderable, light, moving object, etc.).
    
*   Built-in components cover rendering and basic game functionality, while custom components let you add game-specific logic.
    
*   FastCG encourages you to **compose** behavior (add multiple components to one object) rather than relying on deep class inheritance. This makes it easy to have, say, a light that is also moving (Light + a MovementBehaviour), or a UI element that is also interactive (Mesh + custom script), without creating a new subclass for every combination.
    

By following this system, you can build complex scenes in FastCG and still keep the code modular. When contributing to the framework, understanding the ECS will also help in areas like adding new framework features (for example, if someone were to add a PhysicsSystem, it might introduce PhysicsBody components, etc.). FastCG's component-oriented API is a core aspect of its architecture, aimed at making both usage and extension as straightforward as possible.