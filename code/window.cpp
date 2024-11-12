#include "window.h"

#include "types.h"
#include "input.h"

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (msg)
    {
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    } break;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;

    case WM_KILLFOCUS:
    {
        //! NOTE(sbalse): Clear all input state when window loses focus so we don't have zombie key presses
        //! hanging around.
        bool clearPrevFrameInput = true;
        InputClear(clearPrevFrameInput);
    } break;

    case WM_ACTIVATEAPP:
    {
        OutputDebugStringA("WM_ACTIVATEAPP\n");
    } break;

    case WM_SIZE:   //! Window was resized
    {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (!window)
        {
            break;
        }

        window->m_Width = LOWORD(lParam);
        window->m_Height = HIWORD(lParam);

        InvalidateRect(hWnd, nullptr, TRUE); //! NOTE(sbalse): Redraw window after resizing

        return 0;
    } break;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        u8 vkCode = static_cast<u8>(wParam);

        // TODO(sbalse): Is the following code needed?
        /*bool wasDown = ((lParam & (1ll << 30)) != 0); //! Was the same key down before?
        bool isDown = ((lParam & (1ll << 31)) == 0);  //! Is the same key still down?

        //! Early exit when the key is held down
        if (wasDown == isDown)
        {
            break;
        }*/

        bool pressed = (msg == WM_KEYDOWN);
        KeyboardInputUpdate(vkCode, pressed);
    } break;

    case WM_MOUSEMOVE: //! The mouse has moved
    {
        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        POINTS pt = MAKEPOINTS(lParam);

        // If mouse is inside the client region.
        if (pt.x >= 0 && pt.x < window->m_Width && pt.y >= 0 && pt.y < window->m_Height)
        {
            SetMousePosition(pt.x, pt.y);
        }
    } break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    {
        MouseButton button;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
        {
            button = MouseButton::LBUTTON;
        }
        else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
        {
            button = MouseButton::RBUTTON;
        }
        else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
        {
            button = MouseButton::MBUTTON;
        }

        bool isDown = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN);

        MouseInputUpdate(button, isDown);
    } break;

    case WM_PAINT:  //! The application needs to be re-painted
    {
        PAINTSTRUCT paint;
        HDC deviceContext = BeginPaint(hWnd, &paint);

        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        window->ClearScreen(deviceContext);
        window->DrawShadedTriangle(deviceContext);

        EndPaint(hWnd, &paint);

        return 0;
    } break;

    default:
    {
        result = DefWindowProc(hWnd, msg, wParam, lParam);
    }
    }

    return result;
}

void Window::ClearScreen(HDC deviceContext)
{
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));

    RECT rect;
    GetClientRect(m_WindowHandle, &rect);

    FillRect(deviceContext, &rect, blackBrush);

    DeleteObject(blackBrush);
}

void Window::DrawShadedTriangle(HDC deviceContext)
{
    TRIVERTEX vertex[3] = {};
    vertex[0].x = m_Width / 2;
    vertex[0].y = m_Height / 4;
    vertex[0].Red = 0xff00;
    vertex[0].Green = 0x8000;
    vertex[0].Blue = 0x0000;
    vertex[0].Alpha = 0x0000;

    vertex[1].x = m_Width / 4;
    vertex[1].y = 3 * m_Height / 4;
    vertex[1].Red = 0x9000;
    vertex[1].Green = 0x0000;
    vertex[1].Blue = 0x9000;
    vertex[1].Alpha = 0x0000;

    vertex[2].x = 3 * m_Width / 4;
    vertex[2].y = 3 * m_Height / 4;
    vertex[2].Red = 0x9000;
    vertex[2].Green = 0x0000;
    vertex[2].Blue = 0x9000;
    vertex[2].Alpha = 0x0000;

    GRADIENT_TRIANGLE gTriangle = {};
    gTriangle.Vertex1 = 0;
    gTriangle.Vertex2 = 1;
    gTriangle.Vertex3 = 2;

    //! NOTE(sbalse): Draw a shaded triangle.
    GradientFill(deviceContext, vertex, 3, &gTriangle, 1, GRADIENT_FILL_TRIANGLE);
}

Window::Window(int width, int height, LPCWSTR title)
    : m_Width{ width }
    , m_Height{ height }
    , m_Title{ title }
    , m_WindowHandle{ nullptr }
    , m_ClassName{ L"hw3d" } // TODO: Proper multiple windows support?
{
}

bool Window::InitAndShow()
{
    //! NOTE(sbalse): Register window class
    WNDCLASSEX wc =
    {
        .cbSize = sizeof(wc),
        .style = CS_OWNDC,
        .lpfnWndProc = WndProc,
        .hInstance = GetModuleHandle(nullptr),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .lpszClassName = m_ClassName,
        // .hIcon
    };

    if (!RegisterClassEx(&wc))
    {
        // TODO(sbalse): Logging
        return false;
    }

    const LPCWSTR windowTitle = L"HW3D Window";

    const DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | CS_HREDRAW | CS_VREDRAW | CS_OWNDC |
        WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    RECT wr
    {
        .left = 100,
        .top = 100,
        .right = wr.left + m_Width,
        .bottom = wr.top + m_Height
    };

    if (!AdjustWindowRect(&wr, windowStyle, FALSE))
    {
        // TODO(sbalse): Logging
        return false;
    }

    //! NOTE(sbalse): Create window instance
    m_WindowHandle = CreateWindow(
        m_ClassName,
        m_Title, // window title
        windowStyle, // style
        CW_USEDEFAULT, // x
        CW_USEDEFAULT, // y
        wr.right - wr.left, // width
        wr.bottom - wr.top, // height
        nullptr, // parent
        nullptr, // menu
        GetModuleHandle(nullptr), // instance
        nullptr // lpParam
    );
    if (!m_WindowHandle)
    {
        // TODO(sbalse): Logging
        return false;
    }

    SetWindowLongPtr(m_WindowHandle, GWLP_USERDATA, LONG_PTR(this));

    ShowWindow(m_WindowHandle, SW_SHOW);

    return true;
}

void Window::Destroy()
{
    if (m_WindowHandle)
    {
        UnregisterClass(m_ClassName, GetModuleHandle(nullptr));
        DestroyWindow(m_WindowHandle);
    }
}

void Window::SetTitle(LPCWSTR title)
{
    SetWindowText(m_WindowHandle, title);
}

bool Window::ProcessMessages()
{
    MSG msg = {};

    //! NOTE(sbalse): PeekMessage() is non-blocking whereas GetMessage() will block.
    //! We use PeekMessage() instead of GetMessage() because we want to keep running if there are no
    //! messages.
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (msg.message == WM_QUIT)
    {
        return false;
    }

    return true;
}
