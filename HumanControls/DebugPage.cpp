#include "DebugPage.h"

DebugPage::DebugPage()
    : Page(false, false, Page::PageType::DEBUG_PAGE)
{
}

void DebugPage::paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload)
{
    display.clear();
    display.stringSetRegion(0, 0, "batteryVolts:");
    display.intSetRegion(14, 0, payload.getBatteryVoltage());

    display.stringSetRegion(0, 1, "messageIndex:");
    display.intSetRegion(14, 1, payload.getMessageIndex());
}

void DebugPage::clockwise(TShirtCannonPayload &payload)
{
    return;
}
void DebugPage::counterClockwise(TShirtCannonPayload &payload)
{
    return;
}

int DebugPage::rangeFilter(int value)
{
    return 0;
}