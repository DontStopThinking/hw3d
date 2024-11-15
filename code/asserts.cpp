#include "asserts.h"

#include <format>

void ReportAssertionFailure(const char* condition, const char* message, const char* file, u32 line)
{
    char buffer[2048] = {};

    std::format_to_n(
        buffer, sizeof(buffer) - 1,
        "\n[HW3D] Assertion failed: {}, Message: {}, Location: {}:{}\n\n",
        condition, message, file, line);

    OutputDebugStringA(buffer);
}
