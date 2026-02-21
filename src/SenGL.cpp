#include "SenGL.hpp"
#include <vector>
#include <cmath>
#include <fstream>
#include <iostream>

namespace SenGL {

struct Vert { float xyz[3]; float rgb[3]; float uv[2]; };

class Mat4 {
    float m[16];
public:
    Mat4() { identity(); }
    void identity() {
        for(int i=0; i<16; i++) m[i]=0;
        m[0]=m[5]=m[10]=m[15]=1;
    }
    
    void perspective(float fov, float aspect, float near, float far) {
        float tanHalf = tan(fov/2);
        m[0]=1/(aspect*tanHalf); m[4]=0; m[8]=0; m[12]=0;
        m[1]=0; m[5]=1/tanHalf; m[9]=0; m[13]=0;
        m[2]=0; m[6]=0; m[10]=-(far+near)/(far-near); m[14]=-(2*far*near)/(far-near);
        m[3]=0; m[7]=0; m[11]=-1; m[15]=0;
    }
    
    void lookAt(float ex, float ey, float ez, float tx, float ty, float tz) {
        identity();
        m[12] = -ex; m[13] = -ey; m[14] = -ez;
    }
    
    void transform(Vert& v) {
        float x=v.xyz[0], y=v.xyz[1], z=v.xyz[2];
        v.xyz[0] = m[0]*x + m[4]*y + m[8]*z + m[12];
        v.xyz[1] = m[1]*x + m[5]*y + m[9]*z + m[13];
        v.xyz[2] = m[2]*x + m[6]*y + m[10]*z + m[14];
        float w = m[3]*x + m[7]*y + m[11]*z + m[15];
        if(w != 0) { v.xyz[0]/=w; v.xyz[1]/=w; v.xyz[2]/=w; }
    }
};

class ContextImpl : public Context {
    std::vector<float> fb_r, fb_g, fb_b;  // 三个通道
    int width, height;
    float curColor[3];
    Mat4 proj, view;
    
    void viewport(int& sx, int& sy, float nx, float ny) {
        sx = (int)((nx + 1.0f) * 0.5f * width);
        sy = (int)((1.0f - ny) * 0.5f * height);
    }
    
    void drawLine(int x0, int y0, int x1, int y1, float r, float g, float b) {
        int dx = abs(x1-x0), dy = -abs(y1-y0);
        int sx = x0<x1 ? 1:-1, sy = y0<y1 ? 1:-1;
        int err = dx+dy;
        while(true) {
            if(x0>=0 && x0<width && y0>=0 && y0<height) {
                int idx = y0*width + x0;
                fb_r[idx] = r;
                fb_g[idx] = g;
                fb_b[idx] = b;
            }
            if(x0==x1 && y0==y1) break;
            int e2 = 2*err;
            if(e2 >= dy) { err += dy; x0 += sx; }
            if(e2 <= dx) { err += dx; y0 += sy; }
        }
    }
    
    void drawPoint(int x, int y, float r, float g, float b) {
        for(int dx=-2; dx<=2; dx++)
            for(int dy=-2; dy<=2; dy++)
                if(x+dx>=0 && x+dx<width && y+dy>=0 && y+dy<height) {
                    int idx = (y+dy)*width + (x+dx);
                    fb_r[idx] = r;
                    fb_g[idx] = g;
                    fb_b[idx] = b;
                }
    }
    
public:
    ContextImpl(int w, int h) : width(w), height(h), 
        fb_r(w*h,0), fb_g(w*h,0), fb_b(w*h,0) {
        curColor[0]=curColor[1]=curColor[2]=1;
    }
    

    Context& perspective(float fov, float aspect, float near, float far) override {
        proj.perspective(fov, aspect, near, far);
        return *this;
    }

    Context& lookAt(float ex, float ey, float ez, float tx, float ty, float tz) override {
        view.lookAt(ex, ey, ez, tx, ty, tz);
        return *this;
    }

    Context& color(float r, float g, float b) override {
        curColor[0] = r;
        curColor[1] = g;
        curColor[2] = b;
        return *this;
    }
    
    Context& rect(float left, float bottom, float right, float top, float z) override {
        Vert verts[4] = {
            {{left, bottom, z}, {curColor[0],curColor[1],curColor[2]}, {0,0}},
            {{right, bottom, z}, {curColor[0],curColor[1],curColor[2]}, {1,0}},
            {{right, top, z}, {curColor[0],curColor[1],curColor[2]}, {1,1}},
            {{left, top, z}, {curColor[0],curColor[1],curColor[2]}, {0,1}}
        };
        
        Mat4 mvp;
        int pts[4][2];
        for(int i=0; i<4; i++) {
            mvp.transform(verts[i]);
            viewport(pts[i][0], pts[i][1], verts[i].xyz[0], verts[i].xyz[1]);
        }
        
        float r=curColor[0], g=curColor[1], b=curColor[2];
        drawLine(pts[0][0], pts[0][1], pts[1][0], pts[1][1], r,g,b);
        drawLine(pts[1][0], pts[1][1], pts[3][0], pts[3][1], r,g,b);
        drawLine(pts[3][0], pts[3][1], pts[0][0], pts[0][1], r,g,b);
        
        drawLine(pts[1][0], pts[1][1], pts[2][0], pts[2][1], r,g,b);
        drawLine(pts[2][0], pts[2][1], pts[3][0], pts[3][1], r,g,b);
        drawLine(pts[3][0], pts[3][1], pts[1][0], pts[1][1], r,g,b);
        
        for(int i=0; i<4; i++) drawPoint(pts[i][0], pts[i][1], r,g,b);
        return *this;
    }
    
    Context& line(float x0, float y0, float x1, float y1, float z) override {
        Vert v0 = {{x0, y0, z}, {curColor[0],curColor[1],curColor[2]}, {0,0}};
        Vert v1 = {{x1, y1, z}, {curColor[0],curColor[1],curColor[2]}, {1,1}};
        
        Mat4 mvp;
        mvp.transform(v0); mvp.transform(v1);
        
        int sx0, sy0, sx1, sy1;
        viewport(sx0, sy0, v0.xyz[0], v0.xyz[1]);
        viewport(sx1, sy1, v1.xyz[0], v1.xyz[1]);
        
        float r=curColor[0], g=curColor[1], b=curColor[2];
        drawLine(sx0, sy0, sx1, sy1, r,g,b);
        drawPoint(sx0, sy0, r,g,b);
        drawPoint(sx1, sy1, r,g,b);
        
        return *this;
    }
    
    void save(const std::string& filename) override {
        std::ofstream f(filename);
        f << "P3\n" << width << " " << height << "\n255\n";
        for(int y=0; y<height; y++) {
            for(int x=0; x<width; x++) {
                int idx = y*width + x;
                int r = int(fb_r[idx] * 255);
                int g = int(fb_g[idx] * 255);
                int b = int(fb_b[idx] * 255);
                f << r << " " << g << " " << b << " ";
            }
            f << "\n";
        }
    }
};

Window::Window(int w, int h, const std::string& title) {
    impl = new ContextImpl(w, h);
    std::cout << "SenGL Window: " << title << " (" << w << "x" << h << ")\n";
}

Window::~Window() {
    delete static_cast<ContextImpl*>(impl);
}

Context& Window::draw() {
    return *static_cast<ContextImpl*>(impl);
}

Context& Window::camera() {
    return *static_cast<ContextImpl*>(impl);
}

void Window::save(const std::string& filename) {
    static_cast<ContextImpl*>(impl)->save(filename);
}

}