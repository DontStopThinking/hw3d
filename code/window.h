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
    bool m_IsRunning;

    static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    constexpr Window()
        : m_Width{ 0 }
        , m_Height{ 0 }
        , m_Title{ nullptr }
        , m_WindowHandle{ nullptr }
        , m_ClassName{ L"hw3d" } // TODO: Proper multiple windows support?
        , m_IsRunning{ false }
    {
    }

    bool Init(const int width, const int height, const LPCWSTR title);
    void Show() const;
    void Destroy();
    void SetTitle(const LPCWSTR title) const;
    void ProcessMessages();
    HWND GetWindowHandle() const { return m_WindowHandle; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    bool IsRunning() const { return m_IsRunning; }
};
