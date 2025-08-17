# FastCG Documentation

Welcome! This is the (AI generated, human reviewed) documentation for **FastCG** — a lightweight, cross-platform C++ framework for rapid prototyping of 3D applications.

> **Who is this for?**  
> - Developers using FastCG to build 3D apps.  
> - Contributors wishing to improve the codebase (you're very welcome!).

---

## Quick Start

1. **Clone the repo**
   ```bash
   git clone https://github.com/pboechat/FastCG.git
   cd FastCG
   ```

2. **Configure and build (example: Linux GCC + OpenGL + Debug)**
   ```bash
   cmake --preset linux-gcc-opengl-debug
   cmake --build build-linux-gcc-opengl-debug
   ```

3. **Run an example (example: Linux GCC + OpenGL + Debug)**
   ```bash
   ./deploy/Linux/OpenGL/deferred_rendering/deferred_renderingd
   ```

See the [Build System](#build-system) section below for further details.

---

## Documentation Index

- **Overview**
  - [Overview](./overview.md) - What FastCG is, key features, and why contribute.
  - [Project Structure](./project-structure.md) - Repository layout and modules description.

- **Core Concepts**
  - [Component System](./component_system.md) - GameObject, Transforms, Components, etc.
  - [Asset Pipeline](./asset-pipeline.md) - Shaders, textures, 3D models, scene files.

- **Graphics & Rendering**
  - [Graphics System](./graphics-system.md) — Provides a comprehensive abstraction for various graphics APIs (such as OpenGL and Vulkan).
  - [Rendering System](./rendering-system.md) — Outlines both forward and deferred rendering pipelines.

- **Build System**
  - [Building & Running](./building-and-running.md) — Instructions on building and running FastCG-based code.

---

## Examples

- **Deferred Rendering** — G‑buffers, deferred shading, etc.  
- **Bump Mapping** — Normal mapping material setup.  
- **PCSS Shadows** — Percentage closer soft shadows.  
- **Compute Shader Demos** — GPU compute basics.

> Find them under `examples/` and check each example’s `assets/` for shaders and scenes.

---

## Need Help? Found a Bug?

- Open an issue on the repository with steps to reproduce.  
- Improve these docs! If something was unclear, please add a note or example.
- MRs are welcome.

---

### License & Attribution

FastCG is fully open source! See the repository for license information and third‑party attributions.
