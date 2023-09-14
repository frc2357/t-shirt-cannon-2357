#ifndef LINEAR_ACTUATOR_H
#define LINEAR_ACTUATOR_H

#include <Arduino.h>

class LinearActuator
{
    static const int EXTEND;
    static const int RETRACT;

    static const int MAX_POS_MILLIS;
    static const int MIN_POS_MILLIS;

public:
    LinearActuator(int in1, int in2);
    void init();
    void update(int direction);
    void extend();
    void retract();
    void stop();
    bool isMoving(int direction);

private:
    int m_in1, m_in2, m_currentDirection, m_currentPos;
    unsigned long m_lastMillis;
};

#endif