#ifndef LINEAR_ACTUATOR_H
#define LINEAR_ACTUATOR_H

#include <Arduino.h>

class LinearActuator {
    static const int EXTEND;
    static const int RETRACT;

public:
    LinearActuator(int en, int in1, int in2, uint8_t speed);
    void init();
    void update();
    void extend();
    void retract();
    void stop();

private:
    int m_en, m_in1, m_in2, m_speed, m_currentDirection;
};

#endif