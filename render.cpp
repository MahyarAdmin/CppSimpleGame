#include "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\um\wingdi.h"
#include "utility.cpp"

namespace Render{
    const float RENDER_SCALE = 0.01f;

    namespace Data{
        struct render{
            int width, height;
            void* memory = nullptr;

            BITMAPINFO bitmap_info;
        };
        render renderData;
    }

    namespace Basic{
        void render_image(void){
            unsigned int* pixel = (unsigned int*)Render::Data::renderData.memory;
            for(int y = 0; y < Render::Data::renderData.height; y++) {
                for(int x = 0; x < Render::Data::renderData.width; x++) {
                    *pixel++ = 0xff00ff * x + 0x00ff00 * y;
                }
            }
        } 

        void clear_screen(unsigned int color) {
            unsigned int* pixel = (unsigned int*)Render::Data::renderData.memory;
            for(int y = 0; y < Render::Data::renderData.height; y++) {
                for(int x = 0; x < Render::Data::renderData.width; x++) {
                    *pixel++ = color;
                }
            }
        }
    }

    namespace Shape{
        void draw_rectPix(int x0, int y0, int x1, int y1, unsigned int color) {

            x0 = Util::Algorithm::clampData(0, x0, Render::Data::renderData.width);
            x1 = Util::Algorithm::clampData(0, x1, Render::Data::renderData.width);
            y0 = Util::Algorithm::clampData(0, y0, Render::Data::renderData.height);
            y1 = Util::Algorithm::clampData(0, y1, Render::Data::renderData.height);

            for(int y = y0; y < y1; y++) {
                unsigned int* pixel = (unsigned int*)Render::Data::renderData.memory + x0 + y*Render::Data::renderData.width;
                for(int x = x0; x < x1; x++) {
                    *pixel++ = color;
                }
            }
        }

        void draw_rect(float x, float y, float halfSizeX, float halfSizeY, unsigned int color) {

            x *= Render::Data::renderData.height * Render::RENDER_SCALE;
            y *= Render::Data::renderData.height * Render::RENDER_SCALE;
            halfSizeX *= Render::Data::renderData.height * Render::RENDER_SCALE;
            halfSizeY *= Render::Data::renderData.height * Render::RENDER_SCALE;

            x += Render::Data::renderData.width / 2.f;
            y += Render::Data::renderData.height / 2.f;
            
            int x0 = x - halfSizeX;
            int x1 = x + halfSizeX;
            int y0 = y - halfSizeY;
            int y1 = y + halfSizeY;

            draw_rectPix(x0, y0, x1, y1, color);
        }
    }
    
    namespace Algorithm{
        void renderBufferMaxMin(float* x, float* y) {
            float scaleFactor = (float)Render::Data::renderData.height * Render::RENDER_SCALE;

            *x = (float)Render::Data::renderData.width / (2.f * scaleFactor);
            *y = 1.f / (2.f * Render::RENDER_SCALE);
        }
    }
}