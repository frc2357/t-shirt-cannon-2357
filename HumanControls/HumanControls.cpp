#include "HumanControls.h"
#include "Utils.h"

Utils::ControllerStatus HumanControls::status = Utils::ControllerStatus::DISABLED;
Utils::ControllerStatus HumanControls::lastStatus = Utils::ControllerStatus::DISABLED;

HumanControls::HumanControls(TShirtCannonPayload &payload,
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
                             unsigned int enablePin,
                             unsigned int primePin,
                             unsigned int firePin,
                             unsigned int joystickPinVRX,
                             unsigned int xDeadZoneSize,
                             unsigned int joystickMax,
                             unsigned int joystickPinVRY,
                             unsigned int yDeadZoneSize)
    : m_payload(payload),
      m_menuController(encoderPinA, encoderPinB,
                       angleIncrement, angleMin, angleMax, pressureIncrement, pressureMin,
                       pressureMax, durationIncrement, durationMin, durationMax, hangTimerDuration),
      m_pinDebouncer(numButtons), m_enableController(), m_fireController(),
      m_rightStick(joystickPinVRX, xDeadZoneSize, joystickMax),
      m_leftStick(joystickPinVRY, yDeadZoneSize, joystickMax)
{
    this->m_encoderPinSW = encoderPinSW;
    this->m_enablePin = enablePin;
    this->m_primePin = primePin;
    this->m_firePin = firePin;
}

void HumanControls::init()
{
    m_pinDebouncer.addPin(this->m_encoderPinSW, HIGH, INPUT_PULLUP);
    m_pinDebouncer.addPin(this->m_enablePin, HIGH, INPUT_PULLUP);
    m_pinDebouncer.addPin(this->m_primePin, HIGH, INPUT_PULLUP);
    m_pinDebouncer.addPin(this->m_firePin, HIGH, INPUT_PULLUP);
    m_pinDebouncer.begin();
    m_menuController.init(m_payload);
}

void HumanControls::update(uint8_t *messageBuffer)
{
    this->setStatus();
    m_payload.setMessageIndex((m_payload.getMessageIndex() + 1) % 32);

    m_menuController.menuUpdate(m_payload, status == Utils::ControllerStatus::ENABLED);
    m_pinDebouncer.update();

    m_rightStick.update();
    m_leftStick.update();

    double turn, speed;
    turn = m_rightStick.getResult();
    speed = m_leftStick.getResult();

    Utils::setMotors(m_payload, turn, speed);

    // Message type for data from controller
    m_payload.setMessageType(1);
    m_payload.buildTransmission(messageBuffer, 7);
}

void HumanControls::setStatus()
{

    if (m_payload.getStatus() != Utils::ControllerStatus::DISCONNECTED)
    {
        if (m_payload.getError() == 0)
        {
            // Check the robot's status and see if it is different then the controllers.
            // If it is, set the status to the lowest given status. Ex. Robot returns disabled, but controller enabled, so
            // set both statuses to disabled. Robot status and controller status should always be the same.
            // Only exception may be due to firing setting status to enabled after firing.

            if (m_enableController.getIsEnabled())
            {
                if (m_fireController.getIsPrimed())
                {
                    status = Utils::ControllerStatus::PRIMED;
                }
                else
                {
                    status = Utils::ControllerStatus::ENABLED;
                }
            }
            else
            {
                status = Utils::ControllerStatus::DISABLED;
            }
        }
        else
        {
            status = Utils::ControllerStatus::DISABLED;
        }
    }
    else
    {
        status = Utils::ControllerStatus::DISABLED;
    }

    if (status != lastStatus)
    {
        lastStatus = status;
        m_payload.setStatus(status);
        m_menuController.menuRefresh(m_payload);
    }
}

void HumanControls::setError(const char *format, ...)
{
    va_list args;
    char message[32];
    va_start(args, format);
    vsprintf(message, format, args);
    va_end(args);

    status = Utils::ControllerStatus::DISABLED;
    m_payload.setStatus(Utils::ControllerStatus::DISABLED);

    Serial.print("ERROR: ");
    Serial.println(message);
}

// Methods for debouncer
void HumanControls::onPinActivated(int pinNr)
{
    if (pinNr == m_enablePin)
    {
        m_enableController.setIsEnabled(true);
    }
    else if (pinNr == m_primePin)
    {
        if (status == Utils::ControllerStatus::ENABLED)
        {
            m_fireController.setIsPrimed(true);
        }
        else
        {
            m_fireController.setIsPrimed(false);
        }
    }
    else if (pinNr == m_firePin)
    {
        if (HumanControls::status == Utils::ControllerStatus::PRIMED)
        {
            m_fireController.initiateFiring(true);
            m_fireController.setIsPrimed(false);
            m_payload.setStatus(Utils::ControllerStatus::FIRING);
        }
    }
}

void HumanControls::onPinDeactivated(int pinNr)
{

    if (pinNr == m_enablePin)
    {
        m_enableController.setIsEnabled(false);
        m_fireController.setIsPrimed(false);
    }
    else if (pinNr == m_encoderPinSW)
    {
        m_menuController.menuPress(m_payload, (status == Utils::ControllerStatus::ENABLED));
    }
}