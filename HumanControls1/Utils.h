#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <math.h>

class Utils
{
public:
    enum ControllerStatus
    {
        DISCONNECTED = 0,
        DISABLED = 1,
        ENABLED = 2,
        ADJUSTING = 3,
        PRIMED = 4,
        FIRING = 5
    };
    static void setMotors(float turn, float speed);

private:
    static float applyExponent(float value, unsigned int exponent);
};

#endif // UTILS_H