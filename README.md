# SenGL
- SenGL is a renderer that is completely not based on openGL, but it can only draw ppm images. Look forward to other functions!
# Use
The following is the example code
``` cpp
#include "SenGL.hpp"

int main() {
    // Create an 800*600 canvas named SenGL Test
    SenGL::Window win(800, 600, "SenGL Test");
    // Set the camera
    win.camera()
        .lookAt(0,0,5)
        .perspective(60.0f, 800.0f/600.0f);
    // Draw
    win.draw()
        // The main body is red.
        .color(1,0,0)
        .rect(-0.5, -0.5, 0.5, 0.5)
        // Diagonal blue
        .color(0,1,0)
        .line(-0.8, -0.8, 0.8, 0.8);
    // Save as a ppm format image
    win.save("test.ppm");
    return 0;
}
`
# Install
1. build
``` bash
make
`
- You can link this library when you compile c++ files
# About
It's not easy to make. Please give it a star!