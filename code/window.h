#pragma once

#include "cleanwindows.h"

class Window
{
private:
    int m_Width;
    int m_Height;
    // TODO: Proper multiple windows support?
    LPCWSTR m_ApplicationName;
    HWND m_WindowHandle;
    bool m_IsRunning;

    static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    constexpr Window()
        : m_Width{ 0 }
        , m_Height{ 0 }
        , m_ApplicationName{ nullptr }
        , m_WindowHandle{ nullptr }
        , m_IsRunning{ false }
    {
    }

    bool Init(const int width, const int height, const LPCWSTR applicationName);
    void Show() const;
    void Destroy();
    void SetApplicationName(const LPCWSTR name) const;
    void ProcessMessages();
    HWND GetWindowHandle() const { return m_WindowHandle; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    bool IsRunning() const { return m_IsRunning; }
};
