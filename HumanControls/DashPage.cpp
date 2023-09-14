#include "DashPage.h"

DashPage::DashPage() : Page(true, false, Page::PageType::DASH_PAGE)
{
}

void DashPage::paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload)
{
    display.clear();

    if (isActivated)
    {
        display.stringSetRegion(0, 0, "ERROR: ");
        display.intSetRegion(7, 0, payload.getError());
    }
    else
    {
        if (payload.getError() != 0)
        {
            display.stringSetRegion(0, 0, "ERROR");
        }
        else
        {
            display.stringSetRegion(0, 0, "Status:");
            display.intSetRegion(7, 0, payload.getStatus());
        }

        display.stringSetRegion(9, 0, "R:");
        display.intSetRegion(11, 0, payload.getBatteryVoltage());
        display.stringSetRegion(13, 0, "C:");
        display.intSetRegion(15, 0, 5);

        display.stringSetRegion(0, 1, "A:");
        display.intSetRegion(2, 1, payload.getAngle());

        display.stringSetRegion(5, 1, "P:");
        display.intSetRegion(7, 1, payload.getFiringPressure());

        display.stringSetRegion(11, 1, "D:");
        display.intSetRegion(13, 1, (payload.getFiringTime() * 10) + 100);
    }
}

void DashPage::clockwise(TShirtCannonPayload &payload)
{
    return;
}
void DashPage::counterClockwise(TShirtCannonPayload &payload)
{
    return;
}

int DashPage::rangeFilter(int value)
{
    return 0;
}