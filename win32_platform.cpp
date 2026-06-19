#include <windows.h>
#include <functional>
#include "render.cpp"

LRESULT CALLBACK windows_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool runState = true;

class WindowMessage{
    MSG msg = {};
public:
    void messageHandler(void) {
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
};

class Window {
WNDCLASSEXW windows_class = {};
HINSTANCE hInstance = nullptr;
HWND hWnd = nullptr;
HDC hDc = nullptr;

public:
    ~Window() {
        if (hDc && hWnd) {
            ReleaseDC(hWnd, hDc);
        }
        if (hWnd) {
            DestroyWindow(hWnd);
        }
    }
    void setWindowAtt(UINT style, LPCWSTR className, WNDPROC CallBackFunction, HINSTANCE hInst) {
        this->hInstance = hInst;

        windows_class.cbSize = sizeof(WNDCLASSEXW);
        windows_class.style = style;
        windows_class.lpfnWndProc = CallBackFunction;
        windows_class.hInstance = hInst;
        windows_class.lpszClassName = className;
        windows_class.hCursor = LoadCursor(nullptr, IDC_ARROW); 
    }

    void registerWindow() {
        RegisterClassExW(&windows_class);
    }

    bool createWindow(LPCWSTR title, int width, int height) {
        hWnd = CreateWindowExW(
            0,
            windows_class.lpszClassName,
            title,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, 
            width, height,
            nullptr, nullptr, hInstance, nullptr
        );

        if (!hWnd) return false;
        hDc = GetDC(hWnd);
        return true;
    }

    void RunLoopControl(const bool* runState, WindowMessage& msgElement, std::function<void()> insideLoopFunction = []{}){
        while(*runState) {

            msgElement.messageHandler();

            insideLoopFunction();
        }
    }

    HDC getDC() const { return hDc; }
    HWND getHWND() const { return hWnd; }
};

void gamerender(void) {
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd) {

    Window window;
    WindowMessage message;
    window.setWindowAtt(CS_HREDRAW | CS_VREDRAW, L"GameWindowClass", windows_callback, hInstance);
    window.registerWindow();
    if(!window.createWindow(L"My first game", 1280, 720))return -1;
    while(runState) {

        message.messageHandler();

        clear_screen(0xffffff); 
        draw_rect(0, 0, 1, 1, 0x0000ff);
        StretchDIBits(window.getDC(), 0, 0, renderData.width, renderData.height, 0, 0, renderData.width, renderData.height, renderData.memory, &(renderData.bitmap_info), DIB_RGB_COLORS, SRCCOPY);
    }
    

    
    

    return 0;
}

LRESULT CALLBACK windows_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = {};
    switch (uMsg){
        case WM_CLOSE: {
            DestroyWindow(hwnd);
            return 0;
        } break;

        case WM_DESTROY:{
            runState = false;
            PostQuitMessage(0);
            break;
        }

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