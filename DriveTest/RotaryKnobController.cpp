#include "RotaryKnobController.h"

RotaryKnobController::RotaryKnobController(unsigned int pinClk, unsigned int pinDt)
{
    // Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
    this->m_encoder = new RotaryEncoder(pinClk, pinDt, RotaryEncoder::LatchMode::TWO03);

    this->m_pos = 0;
}

RotaryKnobController::~RotaryKnobController()
{
    delete (this->m_encoder);
}

int RotaryKnobController::getValue()
{
    this->m_encoder->tick();
    int newPos = this->m_encoder->getPosition();
    //Dials
    if (m_pos != newPos && newPos % 2 == 0)
    {
        m_pos = newPos;

        //COUNTERCLOCKWISE
        if (this->m_encoder->getDirection() == RotaryEncoder::Direction::CLOCKWISE)
        {
            return -1;
        }

        //CLOCKWISE
        else if (this->m_encoder->getDirection() == RotaryEncoder::Direction::NOROTATION)
        {
            return 1;
        }
    }
    return 0;
}