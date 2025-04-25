#include "ElevatorPage.h"

ElevatorPage::ElevatorPage(int increment, int min, int max)
    : Page(true, false, Page::PageType::ELEVATOR_PAGE)
{
    this->m_min = min;
    this->m_max = max;
    this->m_increment = increment;
}

void ElevatorPage::paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload)
{
    display.clear();

    display.stringSetRegion(1, 0, "Elevator Angle");

    switch (payload.getAngle())
    {
    case 0:
        display.stringSetRegion(7, 1, "vv");
        break;
    case 2:
        display.stringSetRegion(7, 1, "^^");
        break;
    default:
        display.stringSetRegion(7, 1, "--");
        break;
    }

    if (isActivated)
    {
        display.stringSetRegion(4, 1, "**");
        display.stringSetRegion(10, 1, "**");
    }
}

void ElevatorPage::clockwise(TShirtCannonPayload &payload)
{
    int angle = payload.getAngle();
    angle = angle + this->m_increment;
    angle = rangeFilter(angle);
    payload.setAngle(angle);
}
void ElevatorPage::counterClockwise(TShirtCannonPayload &payload)
{
    int angle = payload.getAngle();
    angle = angle - this->m_increment;
    angle = rangeFilter(angle);
    payload.setAngle(angle);
}

void ElevatorPage::onLeave(TShirtCannonPayload &payload)
{
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