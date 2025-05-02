#ifndef DRIVE_PAGE_H
#define DRIVE_PAGE_H

#include "Page.h"
#include "Utils.h"

class DrivePage : public Page
{
public:
    DrivePage();
    void paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload);
    void clockwise(TShirtCannonPayload &payload);
    void counterClockwise(TShirtCannonPayload &payload);
    void onLeave(TShirtCannonPayload &payload);
    int rangeFilter(int value);

    bool isEnabled();

private:
    bool m_isDrivingEnabled = false;
};

#endif // DRIVE_PAGE_H
