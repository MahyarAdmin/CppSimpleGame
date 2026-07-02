//Include
#include <windows.h>
#include <functional>
#include <sstream>
#include <map>
#include <vector>

//local include
#include "render.cpp"

//global variable
bool runState = true;

//global define
const float PLAYER_SPEED = 40.f;
const float PLAYER_SIZE = 5.f;
const float GRAVITY_CONST_SPEED = 0.02f;
const float GRAVITY_SPEED = 0.2f;
const float JUMP_BOND = -20.f;
const float JUMP_SPEED = 2.f;
const float GRAVITY = 30.0f;
const float JUMP_STRENGTH = 45.0f;

//function signature 
LRESULT CALLBACK windows_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void wStringTypeshowMessageOnScreen(HDC hdc, COLORREF textColor, const int mode, int x, int y, LPCWSTR string, const int stringLen);
std::wstring wStringStreamData(std::wstringstream& ss);
void wStringStreamClear(std::wstringstream& ss);

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

class GameCharacter{
float x;
float y;
float playerSize;
float playerSpeed;
public:
    GameCharacter(float __x = 0.f, float __y = 0.f, float __playerSize = 1.f, float __playerSpeed = 1.f) 
    : x(__x), y(__y), playerSize(__playerSize), playerSpeed(__playerSpeed) {}
    float getCordinateX(void) const {
        return x;
    }
    float getCordinateY(void) const {
        return y;
    }
    void setCordinate(const float x, const float y) {
        this -> x = x;
        this -> y = y;
    }
    void setPlayerSize(const float size) {
        playerSize = size;
    }
    float getPlayerSize(void) const {
        return playerSize;
    }
    void setPlayerSpeed(const float speed) {
        playerSpeed = speed;
    }
    float getPlayerSpeed(void) const {
        return playerSpeed;
    }
};

class Cube : public GameCharacter {
unsigned int color;
public:
    void setColor(const unsigned int color) {
        this -> color = color;
    }
    unsigned int getColor(void) const {
        return color;
    }
};

template<class T>
class FPSTimer : public Util::Time::Timer<T> {
public:
    T fpsCount = 0;
    T FPS = 0;
};
//main API function
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd) {

    Window window;
    WindowMessage message;
    std::wstringstream ss;
    Util::Time::Timer<float> frameCounter;
    FPSTimer<float> FPS;
    Util::Time::Timer<float> keyPressOffset;

    window.setWindowAtt(CS_HREDRAW | CS_VREDRAW, L"GameWindowClass", windows_callback, hInstance);
    window.registerWindow();
    if(!window.createWindow(L"My first game", 1280, 720))return -1;

    FPS.SetNow();
    FPS.SetAndSave();
    frameCounter.SetNow();
    frameCounter.SetAndSave();
    keyPressOffset.SetNow();
    keyPressOffset.SetAndSave();

    // std::chrono::high_resolution_clock::time_point frameCounter = std::chrono::high_resolution_clock::now()
    // , frameCounterTemp = std::chrono::high_resolution_clock::now()
    // , time = std::chrono::high_resolution_clock::now()
    // , timeTemp = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<float> Elapsed, frameDuration;
    // int fpsCount = 0;
    // float FPS = 0;

    std::wstring string;
    
    Cube cube1;
    cube1.setColor(0x0000ff);

    float x = 0.f, y = 0.f;
    float maxX = 0.f,maxY = 0.f;
    float playerSpeedDelta = 0.f;
    float playerStep = 0.f;
    bool gravityLock = false, isKeyHold = false;
    float velocityY = 0.0f;
    bool isOnGround = true;

    while(runState) {

        message.messageHandler();

        cube1.setPlayerSize(PLAYER_SIZE);
        cube1.setPlayerSpeed(PLAYER_SPEED);

        Render::Basic::clear_screen(0xffffff);
        Render::Shape::draw_rect(cube1.getCordinateX(), cube1.getCordinateY(), cube1.getPlayerSize(), cube1.getPlayerSize(), 0x0000ff);
        StretchDIBits(window.getDC(), 0, 0, Render::Data::renderData.width, Render::Data::renderData.height, 0, 0, Render::Data::renderData.width, Render::Data::renderData.height, Render::Data::renderData.memory, &(Render::Data::renderData.bitmap_info), DIB_RGB_COLORS, SRCCOPY);

        frameCounter.SetNow();
        playerSpeedDelta = frameCounter.getDuration();
        frameCounter.SetLast();

        FPS.SetNow();
        FPS.fpsCount++;
        if (FPS.getDuration() >= 1.f) {
            FPS.FPS = FPS.fpsCount / FPS.getDuration();
            FPS.fpsCount = 0;
            FPS.SetLast();
        }

        playerStep = cube1.getPlayerSpeed() * playerSpeedDelta;

        float groundLevel = -maxY;

        if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && isOnGround) {
            velocityY = JUMP_STRENGTH;
            isOnGround = false;
        }

        velocityY -= GRAVITY * playerSpeedDelta;
        y += velocityY * playerSpeedDelta;

        if (y <= groundLevel) {
            y = groundLevel;
            velocityY = 0.0f;
            isOnGround = true;
        }

        


        
        // if(GetAsyncKeyState('D') & 0x8000) x += playerStep;
        // if(GetAsyncKeyState('A') & 0x8000) x -= playerStep;
        // if(GetAsyncKeyState('W') & 0x8000) y += playerStep;
        // if(GetAsyncKeyState('S') & 0x8000) y -= playerStep;

        Render::Algorithm::renderBufferMaxMin(&maxX, &maxY);
        maxX -= PLAYER_SIZE;
        maxY -= PLAYER_SIZE;
        x = Util::Algorithm::clampData(-maxX, x, maxX);
        y = Util::Algorithm::clampData(-maxY, y, maxY);

        cube1.setCordinate(x, y);

        ss << L"FPS is:" << FPS.FPS << L", X is:" << x << L", Y is:" << y << L", Window width:" << Render::Data::renderData.width << L", Window height:" << Render::Data::renderData.height;
        string = wStringStreamData(ss);
        wStringTypeshowMessageOnScreen(window.getDC(), RGB(255, 0, 0), TRANSPARENT, 0, 0, string.c_str(), (int)string.length());
        wStringStreamClear(ss);

    }
    
    return 0;
}

//function decelration
void wStringStreamClear(std::wstringstream& ss) {
    ss.str(L"");
    ss.clear();
}

std::wstring wStringStreamData(std::wstringstream& ss) {
    return ss.str();
}

void wStringTypeshowMessageOnScreen(HDC hdc, COLORREF textColor, const int mode, int x, int y, LPCWSTR string, const int stringLen) {
        SetTextColor(hdc, textColor);
        SetBkMode(hdc, mode); 
        TextOutW(hdc, x, y, string, stringLen);
}

LRESULT CALLBACK windows_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = {};
    switch (uMsg){
        // case WM_KEYDOWN: {
        //     if(wParam == VK_UP){
        //         PLAYER_SIZE++;
        //     }
        //     else if(wParam == VK_DOWN) {
        //         PLAYER_SIZE--;
        //     }
        //     else if(wParam == VK_RIGHT) {
        //         PLAYER_SPEED += 10;
        //     }
        //     else if(wParam == VK_LEFT) {
        //         PLAYER_SPEED -= 10;
        //     }
        //     break;
        // }

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

            Render::Data::renderData.width = rect.right - rect.left;
            Render::Data::renderData.height = rect.bottom - rect.top;
            if(Render::Data::renderData.width > 0 && Render::Data::renderData.height > 0) {
                int buffer_windowSize = Render::Data::renderData.height * Render::Data::renderData.width * sizeof(unsigned int);

                if(Render::Data::renderData.memory) VirtualFree(Render::Data::renderData.memory, 0, MEM_RELEASE);
                Render::Data::renderData.memory = VirtualAlloc(0, buffer_windowSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            }
            Render::Data::renderData.bitmap_info.bmiHeader.biSize = sizeof(Render::Data::renderData.bitmap_info.bmiHeader);
            Render::Data::renderData.bitmap_info.bmiHeader.biWidth = Render::Data::renderData.width;
            Render::Data::renderData.bitmap_info.bmiHeader.biHeight = Render::Data::renderData.height;
            Render::Data::renderData.bitmap_info.bmiHeader.biPlanes = 1;
            Render::Data::renderData.bitmap_info.bmiHeader.biBitCount = 32;
            Render::Data::renderData.bitmap_info.bmiHeader.biCompression = BI_RGB;
        } break;

        default: {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result;
}