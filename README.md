# WebGPU 3D Engine

A web-based 3D engine written in C/C++ and compiled to WASM, built around WebGPU for tight memory and performance control. The project pairs a Data-Oriented, ECS-based core with an ImGui-driven editor for development and debugging.

## Project Framing

The engine is an experimental R&D project rather than a production-ready product. It's still in active development, currently used to explore techniques (reflections, ambient occlusion, post-processing) and build WebGPU expertise that can later feed into higher-level tools like Three.js.

## Installation

Not available yet — the project is still in development. Setup instructions will be added once a first usable build is ready.

## Log

**12 december 2025**
Added blit and post-processing effects: bloom, gamma correction, exposure, and dynamic multisample update.

![Post-processing effects — bloom, gamma correction, exposure](docs/images/post-effects.png)

**24 november 2025**
Refined ambient occlusion, speeding up the baking process by 87%.

![Refined ambient occlusion](docs/images/ao-refined.png)

**14 october 2025**
Implemented the Editor UI in C++ with ImGui for faster productivity — tree view, rendering modes (bounding box, wireframe, solid, textured), inspector attributes, plus a log and profiler. Considering a switch to a React-based interface for faster future iteration.

![Editor UI — tree view, inspector, rendering modes](docs/images/editor-ui.png)

**4 september 2025**
Added probe & planar reflections (in progress). Using a Kawase blur to blur the downscaled reflection render — still fairly slow.

![First tries of probe and planar reflections](docs/images/reflections.png)
![Probe and planar reflections](docs/images/reflections-1.png)

**28 august 2025**
Added Gizmo / lights / cameras UI.

![Gizmo, lights and cameras UI](docs/images/gizmo-ui.png)

**5 may 2025**
First attempt at ambient occlusion baking. Slow, and results weren't good enough. Baking AO directly on the model would require a UV editor, which means offering more mesh-editing control than the project's scope and timeline allow — considering baking AO in Blender instead.

![First ambient occlusion baking attempt](docs/images/ao-v1.png)

**19 april 2025**
Implemented shadow mapping.

![Shadow mapping](docs/images/shadow-mapping.png)

**23 march 2025**
Basic geometry loading (glTF) with flat shading.

![Basic geometry loading with flat shading](docs/images/gltf-flat-shading.png)
