#include "SenGL.hpp"

int main() {
    SenGL::Window win(800, 600, "SenGL Test");
    
    win.camera()
        .lookAt(0,0,5)
        .perspective(60.0f, 800.0f/600.0f);
    
    win.draw()
        .color(1,0,0)
        .rect(-0.5, -0.5, 0.5, 0.5)
        .color(0,1,0)
        .line(-0.8, -0.8, 0.8, 0.8);
    
    win.save("test.ppm");
    return 0;
}