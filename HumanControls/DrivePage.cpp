#include "DrivePage.h"

DrivePage::DrivePage() : Page(true, false, Page::PageType::DRIVE_PAGE)
{
}

void DrivePage::paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload)
{
    m_isDrivingEnabled = isActivated;

    display.clear();

    display.stringSetRegion(2, 0, "Drive Status");

    switch (payload.getStatus())
    {
    case Utils::ControllerStatus::ENABLED:
        display.stringSetRegion(4, 1, "Driving");
        break;
    case Utils::ControllerStatus::ADJUSTING:
        display.stringSetRegion(4, 1, "Adjusting");
        break;
    case Utils::ControllerStatus::PRIMED:
        display.stringSetRegion(4, 1, "Primed");
        break;
    case Utils::ControllerStatus::FIRING:
        display.stringSetRegion(4, 1, "Firing");
        break;
    default:
        display.stringSetRegion(4, 1, "Disabled");
        break;
    }

    display.intSetRegion(1, 1, payload.getStatus());
}

void DrivePage::clockwise(TShirtCannonPayload &payload)
{
}

void DrivePage::counterClockwise(TShirtCannonPayload &payload)
{
}

void DrivePage::onLeave(TShirtCannonPayload &payload)
{
    m_isDrivingEnabled = false;
    payload.setStatus(Utils::ControllerStatus::DISABLED);
}

int DrivePage::rangeFilter(int value)
{
    return 0;
}

bool DrivePage::isEnabled()
{
    return m_isDrivingEnabled;
}