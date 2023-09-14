#include "LinearActuator.h"

const int LinearActuator::EXTEND = 1;
const int LinearActuator::RETRACT = 2;

const int LinearActuator::MAX_POS_MILLIS = 8955;
const int LinearActuator::MIN_POS_MILLIS = 455;

LinearActuator::LinearActuator(int in1, int in2)
{
    m_in1 = in1;
    m_in2 = in2;
    m_currentDirection = 0;
}

void LinearActuator::init()
{
    pinMode(m_in1, OUTPUT);
    pinMode(m_in2, OUTPUT);
}

void LinearActuator::update(int direction)
{
    if (m_currentDirection != direction)
    {
        m_currentDirection = direction;

        if (direction == EXTEND)
        {
            extend();
        }
        else if (direction == RETRACT)
        {
            retract();
        }
        else
        {
            stop();
        }
    }

    unsigned long currentMillis = millis();

    if (direction == EXTEND)
    {
        m_currentPos += currentMillis - m_lastMillis;
    }
    else if (direction == RETRACT)
    {
        m_currentPos -= currentMillis - m_lastMillis;
    }
    m_lastMillis = currentMillis;

    if (m_currentPos<MIN_POS_MILLIS | m_currentPos> MAX_POS_MILLIS)
    {
        stop();
    }

    Serial.print("Position: ");
    Serial.println(m_currentPos);
}

bool LinearActuator::isMoving(int direction)
{
    return (direction == EXTEND || direction == RETRACT) || m_lastMillis > 0;
}

void LinearActuator::extend()
{
    m_lastMillis = millis();
    digitalWrite(m_in1, HIGH);
    digitalWrite(m_in2, LOW);
}

void LinearActuator::retract()
{
    m_lastMillis = millis();
    digitalWrite(m_in1, LOW);
    digitalWrite(m_in2, HIGH);
}

void LinearActuator::stop()
{
    m_lastMillis = 0;
    digitalWrite(m_in1, LOW);
    digitalWrite(m_in2, LOW);
}