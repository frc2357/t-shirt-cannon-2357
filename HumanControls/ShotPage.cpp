#include "ShotPage.h"

ShotPage::ShotPage(int increment, int min, int max)
    : Page(true, true, Page::PageType::SHOT_PAGE)
{
    this->m_min = min;
    this->m_max = max;
    this->m_increment = increment;
}

void ShotPage::paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload)
{
    display.clear();

    display.stringSetRegion(1, 0, "Shot Pressure");
    display.stringSetRegion(0, 1, "F:");

    display.intSetRegion(3, 1, payload.getFiringPressure());
    display.stringSetRegion(11, 1, "T:");
    display.intSetRegion(13, 1, payload.getTankPressure());

    if (isActivated)
    {
        display.stringSetRegion(2, 1, "v");
        if (payload.getFiringPressure() >= 100)
        {
            display.stringSetRegion(6, 1, "^");
        }
        else
        {
            display.stringSetRegion(5, 1, "^");
        }
    }
}

void ShotPage::clockwise(TShirtCannonPayload &payload)
{
    uint8_t frPres = payload.getFiringPressure();
    frPres = frPres + this->m_increment;
    frPres = rangeFilter(frPres);
    payload.setFiringPressure(frPres);
}
void ShotPage::counterClockwise(TShirtCannonPayload &payload)
{
    uint8_t frPres = payload.getFiringPressure();
    frPres = frPres - this->m_increment;
    frPres = rangeFilter(frPres);
    payload.setFiringPressure(frPres);
}

int ShotPage::rangeFilter(int value)
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