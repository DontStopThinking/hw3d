#pragma once

#include "cleanwindows.h"
#include "graphics.h"

class Window
{
private:
    int m_Width;
    int m_Height;
    LPCWSTR m_Title;
    HWND m_WindowHandle;
    LPCWSTR m_ClassName;
    bool m_IsRunning;

    static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    Window();

    bool Init(int width, int height, LPCWSTR title);
    void Show();
    void Destroy();
    void SetTitle(LPCWSTR title);
    void ProcessMessages();
    HWND GetWindowHandle() const { return m_WindowHandle; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    bool IsRunning() const { return m_IsRunning; }
};
