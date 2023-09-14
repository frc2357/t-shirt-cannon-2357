#ifndef SHOT_PAGE_H
#define SHOT_PAGE_H

#include "Page.h"
#include <Arduino.h>
#include <JsonElement.h>
#include "TShirtCannonPayload.h"

class ShotPage : public Page
{
public:
    ShotPage(int increment, int min, int max);
    void paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload);
    void clockwise(TShirtCannonPayload &payload);
    void counterClockwise(TShirtCannonPayload &payload);
    int rangeFilter(int value);

private:
    int m_min, m_max, m_increment;
};

#endif