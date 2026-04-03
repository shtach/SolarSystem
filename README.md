# Solar System Simulation

A real-time N-body gravitational simulation of the solar system built from scratch in C++17 with a custom OpenGL 4.3 rendering engine. The project serves as a learning platform for game engine architecture, numerical physics, and GPU programming.

---

## Table of Contents

- [Installation](#installation)
- [Building](#building)
- [Running](#running)
- [Controls](#controls)
- [Technologies](#technologies)
- [Algorithms](#algorithms)
- [Architecture](#architecture)
- [Solar System Data](#solar-system-data)

---

## Installation

### System requirements

- Linux (X11 or Wayland)
- OpenGL 4.3-capable GPU and drivers
- CMake >= 3.21
- Ninja build system
- GCC >= 9 or Clang >= 10 (C++17 required)

### Install system dependencies

**Arch Linux**
```bash
sudo pacman -S base-devel cmake ninja mesa libx11 libxrandr libxinerama libxcursor libxi
```

**Debian / Ubuntu**
```bash
sudo apt update
sudo apt install build-essential cmake ninja-build \
    libgl1-mesa-dev libx11-dev libxrandr-dev \
    libxinerama-dev libxcursor-dev libxi-dev \
    wayland-protocols libwayland-dev libxkbcommon-dev
```

### Clone the repository

```bash
git clone https://github.com/shtach/SolarSystem.git
cd SolarSystem
```

GLFW and GLAD are vendored under `external/` — no additional library installation is needed.

---

## Building

The project uses CMake with three presets. Choose the one that fits your use case.

### Debug build (recommended for development)

```bash
cmake --preset debug
cmake --build --preset debug
```

Output: `build/debug/bin/SolarSystem`

### Release build (optimized, no debug symbols)

```bash
cmake --preset release
cmake --build --preset release
```

Output: `build/release/bin/SolarSystem`

### AddressSanitizer build (for memory error detection)

```bash
cmake --preset asan
cmake --build --preset asan
```

Output: `build/asan/bin/SolarSystem`

### Build presets summary

| Preset | Optimization | Debug symbols | Sanitizers |
|--------|-------------|---------------|------------|
| `debug` | None | Yes | No |
| `release` | -O2 | No | No |
| `asan` | None | Yes | Address + Undefined |

---

## Running

```bash
# From the project root after a debug build
./build/debug/bin/SolarSystem
```


### Log output

The application writes structured logs to the terminal and to `solar_system.log` in the working directory.

```
[12:34:56] [INFO ] [App       ] === SOLAR SYSTEM SIMULATION ===
[12:34:56] [INFO ] [App       ] Bodies: 1023 | Speed: 1000000.000000x
```

---

## Controls

| Input | Action |
|-------|--------|
| W A S D | Move camera |
| Mouse scroll | Zoom in / out |
| Left mouse drag | Pan |
| Space | Pause / unpause simulation |
| R | Reset view to show entire solar system |
| Up arrow | Double simulation speed (max 1,000,000,000x) |
| Down arrow | Halve simulation speed (min 10,000x) |
| 1 | Reset speed to 1,000,000x |
| T | Toggle orbit trails on / off |

### Camera tips

- Press R at any time to return to the full system view.
- Scroll to zoom into a specific planet, then WASD to center on it.
- At maximum zoom-out, the entire solar system including Neptune is visible.
- At maximum zoom-in, individual moons become distinguishable.

---

## Technologies

| Technology | Version | Role |
|------------|---------|------|
| C++ | 17 | Core language |
| OpenGL | 4.3 | Rendering API |
| GLSL | 3.30 core | GPU shader programs |
| GLFW | 3 | Window creation and input handling (vendored) |
| GLAD | 2 | OpenGL function loader (vendored) |
| CMake | >= 3.21 | Build system |
| Ninja | - | Build backend |

---

## Algorithms

### Physics

**N-Body Gravitational Simulation**

Each body exerts a gravitational force on every other body according to Newton's law of universal gravitation:
```math
F = G * {m_1 * m_2 \above 1pt r^2}
```
The gravitational constant used is G = 6.67430e-11 m^3 / (kg * s^2).

**Velocity Verlet Integration**

Positions and velocities are advanced using the Velocity Verlet method rather than Euler integration. It is a second-order symplectic integrator that conserves energy over long time periods, keeping orbital paths stable across millions of simulation steps.

```math
x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt^2 \\
a(t+dt) = {F(x(t+dt)) \above 1pt m} \\
v(t+dt) = v(t) + 0.5*(a(t) + a(t+dt))*dt
```

Physics timestep: 3600 seconds (one simulated hour per step). At default speed (1,000,000x), this produces 4-5 physics steps per rendered frame.

**Barnes-Hut Approximation**

Direct N-body gravity is O(N^2). With 1023 bodies this means over 500,000 force calculations per step. The Barnes-Hut algorithm reduces this to O(N log N) by building a QuadTree over all body positions and treating spatially distant groups as a single mass at their center of mass.

The accuracy/speed tradeoff is controlled by theta (default 0.5). A node is treated as a single mass when:
```
node_width / distance < theta
```

A node pool (flat pre-allocated array) is used instead of heap-allocated nodes to improve cache locality.

**Softening Parameter**

A softening distance of 1e10 m is added to the squared distance in the force denominator. This prevents the force from approaching infinity when two bodies are very close together, avoiding numerical explosion.

### Rendering

**Multi-Pass Rendering**

Each frame renders in four ordered passes to achieve correct visual layering:
1. Starfield (screen-space, static)
2. Atmospheric halos (additive blending)
3. Planet bodies (glow pass + solid core pass)
4. Orbit trails (line strips with per-vertex fade)

**Point Sprites**

All bodies are rendered as `GL_POINTS`. The vertex shader sets `gl_PointSize` per body. The fragment shader uses `gl_PointCoord` to rasterize a circle via `discard` outside the inscribed radius, with a smooth edge from `smoothstep`. This approach efficiently handles 1000+ bodies without per-body triangle meshes.

**Procedural Planet Shading**

The planet fragment shader computes a day/night terminator using the dot product of the surface normal (derived from `gl_PointCoord`) and the direction to the Sun (passed as `uSunPos` world-space uniform). Rocky planets receive value noise for surface terrain detail. Gas giants receive animated horizontal cloud bands from time-dependent sine waves. Jupiter includes a procedural storm spot.

**Trail Fade**

Trail vertex buffers store a `progress` float per vertex (0.0 = oldest point, 1.0 = nearest to body). The fragment shader applies a quadratic falloff `pow(progress, 2.0)` producing trails that fade from transparent at the tail to full color at the head.

**Uniform Caching**

`glGetUniformLocation` performs a string hash lookup in the GPU driver and is called at most once per uniform name. Results are stored in a `std::unordered_map<std::string, GLint>` inside the `Shader` class and reused on every subsequent frame.

---

## Architecture

```
app/          Entry point — constructs Application, calls run()
game/         Application — owns main loop, window, all engine systems
engine/
  core/       Logger, Vec2
  graphics/   Renderer, Camera, Shader
  physics/    Simulation, Body, QuadTree
  input/      InputHandler
assets/
  shaders/    GLSL source files, cop. to bin. dir. at build time
external/
  glfw/       Vendored GLFW
  glad/       Vendored GLAD
```

**Layer rule:** `app` depends on `game`, `game` depends on `engine`. The engine has no dependency on the application layer and can be reused independently.

**Memory management:** All OpenGL resources (shader programs, VAOs, VBOs) follow RAII. Constructors acquire resources, destructors release them. Classes holding GPU resources are non-copyable and movable (`Shader`, `Renderer`).

**Logger:** A Meyer's Singleton with five levels (TRACE, DEBUG, INFO, WARN, ERROR), ANSI terminal color output, optional file output, and `std::mutex` for thread-safe writes. Engine subsystems use it in place of raw `std::cerr` and `std::cout`.

---

## Solar System Data

Orbital velocities are computed from the vis-viva equation:
```math
v = \sqrt{G * M_s \above 1pt r}
```

| Body | Mass (kg) | Orbital radius (m) | Moons |
|------|-----------|--------------------|-------|
| Sun | 1.9885e30 | — | — |
| Mercury | 3.3011e23 | 5.791e10 | 0 |
| Venus | 4.8675e24 | 1.082e11 | 0 |
| Earth | 5.972e24 | 1.496e11 | 1 |
| Mars | 6.4171e23 | 2.279e11 | 2 |
| Jupiter | 1.8982e27 | 7.785e11 | 4 |
| Saturn | 5.6834e26 | 1.429e12 | 3 |
| Uranus | 8.6810e25 | 2.871e12 | 3 |
| Neptune | 1.02413e26 | 4.498e12 | 1 |

Asteroid belt: ~1000 bodies between 3.0e11 m and 5.0e11 m from the Sun.

---

## License

MIT License — see `LICENSE` for details.
