# 🪐 Solar System Simulation

A real-time solar system simulation built in C++17 using OpenGL, GLFW, GLAD, and GLM. The project features physically-based gravitational simulation and an interactive renderer with orbit trails, atmospheric effects, and planetary rings.

## 📁 Project Structure

```
SolarSystem/
├── app/                        # Entry point
│   ├── main.cpp
│   └── CMakeLists.txt
├── engine/                     # Reusable engine library
│   ├── include/engine/
│   │   ├── graphics/           # Renderer, Camera
│   │   ├── physics/            # Body, Simulation
│   │   └── input/              # InputHandler
│   ├── src/                    # Engine implementation
│   └── CMakeLists.txt
├── game/                       # Application layer
│   ├── Application.h / .cpp    # Main loop, system orchestration
│   └── CMakeLists.txt
├── assets/
│   └── shaders/                # GLSL shader files
├── external/
│   ├── glad/                   # OpenGL function loader (GLAD 2)
│   └── glfw/                   # Windowing & input (vendored)
├── CMakeLists.txt
├── CMakePresets.json
└── .gitignore
```

## 🛠️ Dependencies

| Library | Purpose |
|---------|---------|
| **OpenGL 4.3** | Graphics rendering API |
| **GLFW 3** | Window creation & input handling (vendored) |
| **GLAD 2** | OpenGL function loader (vendored) |
| **GLM** | Mathematics library for transformations |

GLFW and GLAD are vendored under `external/` — no separate installation needed.

## ⚙️ Building

### Prerequisites

- CMake ≥ 3.21
- Ninja build system
- C++17-compatible compiler (GCC, Clang)
- OpenGL 4.3-capable GPU & drivers

### Linux

```bash
# Install system dependencies (X11, Wayland, OpenGL)
sudo pacman -S mesa libx11 libxrandr libxinerama libxcursor libxi   # Arch
# or
sudo apt install libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev wayland-protocols libwayland-dev libxkbcommon-dev   # Debian/Ubuntu

# Configure and build
cmake --preset debug
cmake --build build/debug

./build/debug/bin/SolarSystem
```

### Build presets

```bash
cmake --preset debug    # debug symbols, no optimization
cmake --preset release  # optimized
cmake --preset asan     # AddressSanitizer enabled
```

## 🎮 Controls

| Key | Action |
|-----|--------|
| `WASD` | Move camera |
| Mouse wheel | Zoom |
| Left drag | Pan |
| `Space` | Pause / unpause |
| `R` | Reset view to whole system |
| `↑ / ↓` | Increase / decrease simulation speed |
| `1` | Reset to normal speed |
| `C` | Toggle compute shader |
| `T` | Toggle orbit trails |

## 🧩 Architecture

- **`engine`** — static library; self-contained physics, rendering, and input modules with no dependency on application code
- **`game`** — `Application` class that owns the main loop, window lifecycle, and wires engine systems together
- **`app`** — thin entry point that constructs and runs `Application`
- **Physics** — gravitational N-body simulation with numerical integration; index-based body access for vector safety
- **Renderer** — OpenGL 4.3 pipeline with dedicated shaders for bodies, orbit trails, atmospheric glow, and planetary rings
- **Shaders** — all GLSL extracted to `assets/shaders/` and copied to the binary directory at build time

## 📄 License

MIT License — see `LICENSE` for details.