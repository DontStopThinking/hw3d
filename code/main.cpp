#include <cstdlib>
#include <cstdio>

#include "cleanwindows.h"
#include "control.h"

int APIENTRY WinMain(
    _In_ HINSTANCE /*hInstance*/,
    _In_opt_ HINSTANCE /*hPrevInstance*/,
    _In_ LPSTR /*lpCmdLine*/,
    _In_ int /*nCmdShow*/)
{
    ControlInit();

    // NOTE(sbalse): Main engine loop.
    while (ControlRun())
    {
    }

    ControlShutdown();

    return EXIT_SUCCESS;
}
