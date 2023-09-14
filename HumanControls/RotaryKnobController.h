#ifndef ENCODERCONTROLLER_H
#define ENCODERCONTROLLER_H

#include <RotaryEncoder.h>

class RotaryKnobController
{
public:
    RotaryKnobController(unsigned int pinA, unsigned int pinB);
    ~RotaryKnobController();
    int getValue();

private:
    int m_pos = 0;
    RotaryEncoder *m_encoder;
};
#endif