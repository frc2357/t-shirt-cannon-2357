#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H

#include <Arduino.h>
#include <JsonElement.h>
#include "Page.h"
#include "DashPage.h"
#include "ElevatorPage.h"
#include "ShotPage.h"
#include "ValvePage.h"
#include "DebugPage.h"
#include "RotaryKnobController.h"
#include "DisplayController.h"
#include "TShirtCannonPayload.h"
#include "Utils.h"

class MenuController
{
public:
    MenuController(unsigned int encoderPinA,
                   unsigned int encoderPinB,
                   unsigned int angleIncrement,
                   unsigned int angleMin,
                   unsigned int angleMax,
                   unsigned int pressureIncrement,
                   unsigned int pressureMin,
                   unsigned int pressureMax,
                   unsigned int durationIncrement,
                   unsigned int durationMin,
                   unsigned int durationMax,
                   unsigned int hangTimerDuration);
    void init(TShirtCannonPayload &payload);
    void menuUpdate(TShirtCannonPayload &payload, bool isEnabled);
    void menuRefresh(TShirtCannonPayload &payload);
    void menuPress(TShirtCannonPayload &payload, bool isEnabled);

private:
    RotaryKnobController m_rotaryKnob;
    DisplayController m_display;
    int m_rotation, m_hangTimerDuration;
    long m_time;
    bool m_isActive;
    Page *m_currentPage;
    DashPage m_dashPage;
    ElevatorPage m_elevatorPage;
    ShotPage m_shotPage;
    DebugPage m_debugPage;
    ValvePage m_valvePage;
};
#endif