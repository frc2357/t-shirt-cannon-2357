#include "ElevatorPage.h"

ElevatorPage::ElevatorPage(int increment, int min, int max)
    : Page(true, true, Page::PageType::ELEVATOR_PAGE)
{
    this->m_min = min;
    this->m_max = max;
    this->m_increment = increment;
}

void ElevatorPage::paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload)
{
    display.clear();

    display.stringSetRegion(1, 0, "Elevator Angle");
    display.intSetRegion(7, 1, payload.getAngle());

    if (isActivated)
    {
        display.stringSetRegion(6, 1, "^");
        display.stringSetRegion(9, 1, "v");
    }
}

void ElevatorPage::clockwise(TShirtCannonPayload &payload)
{
    uint8_t angle = payload.getAngle();
    angle = angle + this->m_increment;
    angle = rangeFilter(angle);
    payload.setAngle(angle);
}
void ElevatorPage::counterClockwise(TShirtCannonPayload &payload)
{
    uint8_t angle = payload.getAngle();
    angle = angle - this->m_increment;
    angle = rangeFilter(angle);
    payload.setAngle(angle);
}

int ElevatorPage::rangeFilter(int value)
{
    if (value < this->m_min)
    {
        return this->m_min;
    }
    else if (value > this->m_max)
    {
        return this->m_max;
    }
    return value;
}