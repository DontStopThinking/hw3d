#pragma once

// NOTE(sbalse): If x is not nullptr then Release() x and set it to nullptr.
#define SAFE_RELEASE(x) \
    do { \
        if (x) \
        { \
            (x)->Release(); \
            (x) = nullptr; \
        } \
    } while (0) \
