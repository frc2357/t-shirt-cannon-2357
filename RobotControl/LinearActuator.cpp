#include "LinearActuator.h"

const int LinearActuator::EXTEND = 1;
const int LinearActuator::RETRACT = 2;

LinearActuator::LinearActuator(int en, int in1, int in2, uint8_t speed) {
    m_en = en;
    m_in1 = in1;
    m_in2 = in2;
    m_speed = speed;
    m_currentDirection = 0;
}

void LinearActuator::init() {
    pinMode(m_en, OUTPUT);
    pinMode(m_in1, OUTPUT);
    pinMode(m_in2, OUTPUT);
}

void LinearActuator::update(int direction) {
    if(m_currentDirection != direction) {
        m_currentDirection = direction;

        if(direction == EXTEND) {
            extend();
        } else if(direction == RETRACT) {
            retract();
        } else {
            stop();
        }
    }
}

void LinearActuator::extend() {
    digitalWrite(m_in1, HIGH);
    digitalWrite(m_in2, LOW);
    analogWrite(m_en, m_speed);
}

void LinearActuator::retract() {
    digitalWrite(m_in1, LOW);
    digitalWrite(m_in2, HIGH);
    analogWrite(m_en, m_speed);
}

void LinearActuator::stop() {
    analogWrite(m_en, 0);
    m_timerMillis = 0;
    m_startMillis = 0;
}