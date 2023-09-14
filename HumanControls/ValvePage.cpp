#include "ValvePage.h"

ValvePage::ValvePage(int increment, int min, int max)
    : Page(true, true, Page::PageType::VALVE_PAGE)
{
    this->m_min = min;
    this->m_max = max;
    this->m_increment = increment;
}

void ValvePage::paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload)
{
    display.clear();

    display.stringSetRegion(1, 0, "Valve Duration");
    display.intSetRegion(6, 1, (payload.getFiringTime() * 10) + 100);

    if (isActivated)
    {
        display.stringSetRegion(5, 1, "^");
        display.stringSetRegion(9, 1, "v");
    }
}
void ValvePage::clockwise(TShirtCannonPayload &payload)
{
    int vlvTm = payload.getFiringTime();
    vlvTm = vlvTm + this->m_increment;
    vlvTm = rangeFilter(vlvTm);
    payload.setFiringTime(vlvTm);
}
void ValvePage::counterClockwise(TShirtCannonPayload &payload)
{
    int vlvTm = payload.getFiringTime();
    vlvTm = vlvTm - this->m_increment;
    vlvTm = rangeFilter(vlvTm);
    payload.setFiringTime(vlvTm);
}

int ValvePage::rangeFilter(int value)
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