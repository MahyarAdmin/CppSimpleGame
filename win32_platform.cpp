#include <windows.h>
#include "render.cpp"

bool running = true;

LRESULT CALLBACK windows_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd) {

    WNDCLASS windows_class = {};
    windows_class.style = CS_HREDRAW | CS_VREDRAW;
    windows_class.lpszClassName = TEXT("SimpleCppGame");
    windows_class.lpfnWndProc = windows_callback;

    RegisterClass(&windows_class);

    HWND window =  CreateWindow(windows_class.lpszClassName, TEXT("My first ever game"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
    HDC hdc = GetDC(window);

    while(running) {
        MSG msg = {};
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        clear_screen(0x000000);
        draw_rect(0, 0, 10, 10, 0xff5500);
        StretchDIBits(hdc, 0, 0, renderData.width, renderData.height, 0, 0, renderData.width, renderData.height, renderData.memory, &(renderData.bitmap_info), DIB_RGB_COLORS, SRCCOPY);
    }
    return 0;
}

LRESULT CALLBACK windows_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = {};
    switch (uMsg){
        case WM_CLOSE:
        case WM_DESTROY: {
            running = false;
            return 0;
        } break;

        case WM_SIZE: {
            RECT rect;

            GetClientRect(hwnd, &rect);

            renderData.width = rect.right - rect.left;
            renderData.height = rect.bottom - rect.top;

            int buffer_windowSize = renderData.height * renderData.width * sizeof(unsigned int);

            if(renderData.memory) VirtualFree(renderData.memory, 0, MEM_RELEASE);
            renderData.memory = VirtualAlloc(0, buffer_windowSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            renderData.bitmap_info.bmiHeader.biSize = sizeof(renderData.bitmap_info.bmiHeader);
            renderData.bitmap_info.bmiHeader.biWidth = renderData.width;
            renderData.bitmap_info.bmiHeader.biHeight = renderData.height;
            renderData.bitmap_info.bmiHeader.biPlanes = 1;
            renderData.bitmap_info.bmiHeader.biBitCount = 32;
            renderData.bitmap_info.bmiHeader.biCompression = BI_RGB;
        } break;

        default: {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result;
}