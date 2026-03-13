# 🪐 Solar System Simulation

A real-time 3D solar system simulation built in C++17 using OpenGL, GLFW, GLAD, and GLM. The project features physically-based gravitational simulation and an interactive 3D renderer.

## 📁 Project Structure
```
SolarSystem/
├── Math/
│   └── Vec2.h                  # 2D vector math utilities
├── Physics/
│   ├── Body.h / Body.cpp       # Celestial body representation
│   └── Simulation.h / Simulation.cpp  # Physics & gravity engine
├── Rendering/
│   ├── Camera.h                # Camera system
│   ├── InputHandler.h / .cpp   # Keyboard & mouse input
│   └── Renderer.h / .cpp       # OpenGL rendering pipeline
├── Application.h / .cpp        # Main application loop
├── main.cpp                    # Entry point
├── CMakeLists.txt
└── .gitignore
```

## 🛠️ Dependencies

| Library | Purpose |
|--------|---------|
| **OpenGL** | Graphics rendering API |
| **GLFW** | Window creation & input handling |
| **GLAD** (`GLRW/`) | OpenGL function loader |
| **GLM** | Mathematics library for 3D transformations |

## ⚙️ Building

### Prerequisites

- CMake ≥ 3.15
- C++17-compatible compiler (GCC, Clang, MSVC)
- OpenGL-capable GPU & drivers

### Linux
```bash
sudo apt install libglfw3-dev libgl1-mesa-dev libx11-dev
mkdir build && cd build
cmake ..
make -j$(nproc)
./bin/SolarSystemSimulation
```

### Windows (MSVC)
```bat
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
bin\Release\SolarSystemSimulation.exe
```

### macOS
```bash
brew install glfw cmake
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.logicalcpu)
./bin/SolarSystemSimulation
```

### Build types
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug    # with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Release  # optimized (default)
```

## 🧩 Architecture

- **Physics engine** — gravitational N-body simulation with numerical integration
- **Renderer** — OpenGL-based 3D rendering with camera controls
- **Input system** — real-time keyboard and mouse handling via GLFW
- **Application layer** — orchestrates the simulation loop, timing, and state

## 📄 License

MIT License — see `LICENSE` for details.
