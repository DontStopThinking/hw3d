#pragma once

#include "cleanwindows.h"

class Window
{
private:
    int m_Width;
    int m_Height;
    LPCWSTR m_Title;
    HWND m_WindowHandle;
    LPCWSTR m_ClassName;

    static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

    void ClearScreen(HDC deviceContext);
    void DrawShadedTriangle(HDC deviceContext);

public:
    Window(int width, int height, LPCWSTR title);

    bool Init();
    void Destroy();
    void SetTitle(LPCWSTR title);
};
