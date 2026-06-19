#include "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\um\wingdi.h"
#include "utility.cpp"

const float RENDER_SCALE = 0.01f;

struct render{
    int width, height;
    void* memory = nullptr;

    BITMAPINFO bitmap_info;
};
render renderData;


void render_image(void){
    unsigned int* pixel = (unsigned int*)renderData.memory;
    for(int y = 0; y < renderData.height; y++) {
        for(int x = 0; x < renderData.width; x++) {
            *pixel++ = 0xff00ff * x + 0x00ff00 * y;
        }
    }
} 

void clear_screen(unsigned int color) {
    unsigned int* pixel = (unsigned int*)renderData.memory;
    for(int y = 0; y < renderData.height; y++) {
        for(int x = 0; x < renderData.width; x++) {
            *pixel++ = color;
        }
    }
}

void draw_rectPix(int x0, int y0, int x1, int y1, unsigned int color) {

    x0 = clampData(0, x0, renderData.width);
    x1 = clampData(0, x1, renderData.width);
    y0 = clampData(0, y0, renderData.height);
    y1 = clampData(0, y1, renderData.height);

    for(int y = y0; y < y1; y++) {
        unsigned int* pixel = (unsigned int*)renderData.memory + x0 + y*renderData.width;
        for(int x = x0; x < x1; x++) {
            *pixel++ = color;
        }
    }
}

void draw_rect(float x, float y, float halfSizeX, float halfSizeY, unsigned int color) {

    x *= renderData.height * RENDER_SCALE;
    y *= renderData.height * RENDER_SCALE;
    halfSizeX *= renderData.height * RENDER_SCALE;
    halfSizeY *= renderData.height * RENDER_SCALE;

    x += renderData.width / 2.f;
    y += renderData.height / 2.f;
    
    int x0 = x - halfSizeX;
    int x1 = x + halfSizeX;
    int y0 = y - halfSizeY;
    int y1 = y + halfSizeY;

    draw_rectPix(x0, y0, x1, y1, color);
}

void renderBufferMaxMin(int* x, int* y) {
    float scaleFactor = renderData.height * RENDER_SCALE;

    *x = renderData.width / (2.f * scaleFactor);
    *y = 1.f / (2.f * RENDER_SCALE);
}