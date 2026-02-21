#ifndef SENGL_HPP
#define SENGL_HPP

#include <string>

namespace SenGL {

class Context {
public:
    virtual ~Context() = default;
    
    virtual Context& perspective(float fov, float aspect, float near=0.1f, float far=100.0f) = 0;
    virtual Context& lookAt(float ex, float ey, float ez, float tx=0, float ty=0, float tz=0) = 0;
    
    virtual Context& color(float r, float g, float b) = 0;
    
    virtual Context& rect(float left, float bottom, float right, float top, float z=2.0f) = 0;
    virtual Context& line(float x0, float y0, float x1, float y1, float z=2.0f) = 0;
    
    virtual void save(const std::string& filename) = 0;
};

class Window {
public:
    Window(int w, int h, const std::string& title);
    ~Window();
    
    Context& draw();
    Context& camera();
    void save(const std::string& filename);
    
private:
    void* impl;
};

}

#endif