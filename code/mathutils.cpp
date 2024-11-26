#include "mathutils.h"

float PingPong(const float value, const float min, const float max, const float speed)
{
    static int direction = 1; // NOTE(sbalse): 1 = right, -1 = left
    float result = value + (speed * direction);

    if (result >= max)
    {
        result = max;
        direction = -1;
    }
    else if (result <= min)
    {
        result = min;
        direction = 1;
    }

    return result;
}
