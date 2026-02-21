# SenGL

SenGL is a lightweight CPU-based software rendering engine written from scratch in C++.
It supports 2D rendering, perspective camera, true color RGB, and can be compiled as a shared library.

## Features

- True color RGB rendering
- Perspective projection camera
- 2D primitives: rectangle, line, point
- PPM image output
- Shared library support (.so)
- Simple and clean C++ API
- CMake build system

## Usage Example
```cpp
#include "SenGL.hpp"

int main() {
    // Create a 800x600 window
    SenGL::Window win(800, 600, "SenGL Test");
    
    // Set up camera: position and perspective
    win.camera()
        .lookAt(0, 0, 5)
        .perspective(60.0f, 800.0f / 600.0f);
    
    // Draw shapes with color
    win.draw()
        .color(1, 0, 0)          // Set color to red
        .rect(-0.5, -0.5, 0.5, 0.5)  // Draw a rectangle
        .color(0, 1, 0)          // Set color to green
        .line(-0.8, -0.8, 0.8, 0.8); // Draw a line
    
    // Save result to a PPM image file
    win.save("test.ppm");
    return 0;
}
