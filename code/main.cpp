#include <cstdlib>
#include <cstdio>

#include "cleanwindows.h"
#include "window.h"
#include "input.h"
#include "engine.h"

int APIENTRY WinMain(
    _In_ HINSTANCE /*hInstance*/,
    _In_opt_ HINSTANCE /*hPrevInstance*/,
    _In_ LPSTR /*lpCmdLine*/,
    _In_ int /*nCmdShow*/)
{
    Window window(1280, 720, L"HW3D Engine");

    if (!window.InitAndShow())
    {
        // TODO(sbalse): Logging
        return EXIT_FAILURE;
    }

    while (window.ProcessMessages())
    {
        DoFrame();

        EndFrame();
    }

    window.Destroy();

    return EXIT_SUCCESS;
}
