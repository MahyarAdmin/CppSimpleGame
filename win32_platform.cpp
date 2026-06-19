//Include
#include <windows.h>
#include <functional>
#include <sstream>
#include <chrono>

//local include
#include "render.cpp"

//global variable
bool runState = true;

//global define
const float PLAYER_SPEED = 500.f;
const float PLAYER_SIZE = 5.f;

//function signature 
LRESULT CALLBACK windows_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//class
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

//main API function
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd) {

    Window window;
    WindowMessage message;
    std::wstringstream ss;
    window.setWindowAtt(CS_HREDRAW | CS_VREDRAW, L"GameWindowClass", windows_callback, hInstance);
    window.registerWindow();
    if(!window.createWindow(L"My first game", 1280, 720))return -1;


    int x = 0, y = 0;
    int maxX = 0,maxY = 0; 

    std::chrono::high_resolution_clock::time_point frameCounter, frameCounterTemp, time, timeTemp = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> Elapsed, frameDuration;
    int fpsCount = 0;
    float FPS;
    

    while(runState) {

        message.messageHandler();

        clear_screen(0xffffff); 
        draw_rect(x, y, PLAYER_SIZE, PLAYER_SIZE, 0x0000ff);
        StretchDIBits(window.getDC(), 0, 0, renderData.width, renderData.height, 0, 0, renderData.width, renderData.height, renderData.memory, &(renderData.bitmap_info), DIB_RGB_COLORS, SRCCOPY);
        
        fpsCount++;

        frameCounter = time = std::chrono::high_resolution_clock::now();

        frameDuration = frameCounter - frameCounterTemp;
        float playerSpeedDelta = frameDuration.count();

        frameCounterTemp = frameCounter;

        Elapsed = time - timeTemp;

        if(Elapsed.count() >= 1.f) {
            FPS = (float)fpsCount / Elapsed.count();
            fpsCount = 0;
            timeTemp = time;
        }

        if(GetAsyncKeyState('D') & 0x8000) x += (PLAYER_SPEED * playerSpeedDelta);
        if(GetAsyncKeyState('A') & 0x8000) x -= (PLAYER_SPEED * playerSpeedDelta);
        if(GetAsyncKeyState('W') & 0x8000) y += (PLAYER_SPEED * playerSpeedDelta);
        if(GetAsyncKeyState('S') & 0x8000) y -= (PLAYER_SPEED * playerSpeedDelta);

        renderBufferMaxMin(&maxX, &maxY);
        maxX -= PLAYER_SIZE;
        maxY -= PLAYER_SIZE;
        x = clampData(-maxX, x, maxX);
        y = clampData(-maxY, y, maxY);

        ss << L"FPS is:" << (int)FPS << L", X is:" << x << L", Y is:" << y << L", Window width:" << renderData.width << L", Window height:" << renderData.height;
        std::wstring string = ss.str();

        SetTextColor(window.getDC(), RGB(255, 0, 0));
        SetBkMode(window.getDC(), TRANSPARENT); 
        TextOutW(window.getDC(), 20, 20, string.c_str(), (int) string.length());

        ss.str(L"");
        ss.clear();
    }
    
    return 0;
}

//function decelration
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