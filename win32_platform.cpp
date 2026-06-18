#include <windows.h>

bool running = true;
int buffer_width, buffer_height;
void *memoryBuffer;
BITMAPINFO bitmap_info;

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

        unsigned int* pixel = (unsigned int*)memoryBuffer;
        for(int y = 0; y < buffer_height; y++) {
            for(int x = 0; x < buffer_width; x++) {
                *pixel++ = 0xff00ff * x + 0x00ff00 * y;
            }
        }

        StretchDIBits(hdc, 0, 0, buffer_width, buffer_height, 0, 0, buffer_width, buffer_height, memoryBuffer, &bitmap_info, DIB_RGB_COLORS, SRCCOPY);
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

            buffer_width = rect.right - rect.left;
            buffer_height = rect.bottom - rect.top;

            int buffer_windowSize = buffer_height * buffer_width * sizeof(unsigned int);

            if(memoryBuffer) VirtualFree(memoryBuffer, 0, MEM_RELEASE);
            memoryBuffer = VirtualAlloc(0, buffer_windowSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
            bitmap_info.bmiHeader.biWidth = buffer_width;
            bitmap_info.bmiHeader.biHeight = buffer_height;
            bitmap_info.bmiHeader.biPlanes = 1;
            bitmap_info.bmiHeader.biBitCount = 32;
            bitmap_info.bmiHeader.biCompression = BI_RGB;
        } break;

        default: {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result;
}