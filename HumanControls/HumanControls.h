#ifndef HUMAN_CONTROLS_H
#define HUMAN_CONTROLS_H

#include <Arduino.h>
#include <JsonElement.h>
#include "MenuController.h"
#include "FTDebouncer.h"
#include "EnableController.h"
#include "FireController.h"
#include "JoystickAxis.h"
#include "RFM95C.h"
#include "Utils.h"
#include "CommunicationDriver.h"

class HumanControls
{
public:
    HumanControls(TShirtCannonPayload &payload,
                  unsigned int encoderPinA,
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
                  unsigned int hangTimerDuration,
                  unsigned int numButtons,
                  unsigned int encoderPinSW,
                  unsigned int primePin,
                  unsigned int enablePin,
                  unsigned int firePin,
                  unsigned int joystickPinVRX,
                  unsigned int xDeadZoneSize,
                  unsigned int joystickMax,
                  unsigned int joystickPinVRY,
                  unsigned int yDeadZoneSize);
    void init();
    void update(uint8_t *messageBuffer);
    void setStatus();
    void onPinActivated(int pinNr);
    void onPinDeactivated(int pinNr);

    static Utils::ControllerStatus status;

private:
    void setError(const char *format, ...);

    int m_encoderPinSW, m_enablePin, m_primePin, m_firePin;
    static Utils::ControllerStatus lastStatus;

    TShirtCannonPayload &m_payload;

    MenuController m_menuController;
    FTDebouncer m_pinDebouncer;
    EnableController m_enableController;
    FireController m_fireController;
    JoystickAxis m_leftStick, m_rightStick;
};

#endif
