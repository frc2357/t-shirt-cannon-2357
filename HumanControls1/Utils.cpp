#include "Utils.h"

#define MAX_DRIVE_SPEED 63
#define TURN_RAMP_RATE_EXPONENT 3
#define SPEED_RAMP_RATE_EXPONENT 2

void Utils::setMotors(TShirtCannonPayload &payload, float turn, float speed)
{
    turn = applyExponent(turn, TURN_RAMP_RATE_EXPONENT);
    speed = applyExponent(speed, SPEED_RAMP_RATE_EXPONENT);

    double left, right;
    uint8_t leftSpeed, rightSpeed;
    uint8_t leftDir, rightDir;
    uint8_t leftFinal, rightFinal;

    left = turn - speed;
    right = -turn - speed;

    // 1 if negative, otherwise 0
    leftDir = constrain(left, 1, 0);
    rightDir = constrain(right, 1, 0);

    leftSpeed = MAX_DRIVE_SPEED * abs(left);
    rightSpeed = MAX_DRIVE_SPEED * abs(right);

    leftSpeed = constrain(leftSpeed, 0, MAX_DRIVE_SPEED);
    rightSpeed = constrain(rightSpeed, 0, MAX_DRIVE_SPEED);

    leftFinal = (leftDir << 6) | leftSpeed;
    rightFinal = (1 << 7) | (rightDir << 6) | rightSpeed;

    payload.setControllerDriveLeft(leftFinal);
    payload.setControllerDriveRight(rightFinal);
}

float Utils::applyExponent(float value, unsigned int exponent)
{
    return copysign(pow(value, exponent), value);
}