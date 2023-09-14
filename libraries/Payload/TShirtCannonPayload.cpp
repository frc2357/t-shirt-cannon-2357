#include "TShirtCannonPayload.h"

const int TShirtCannonPayload::PAYLOAD_LENGTH = 7;
const int TShirtCannonPayload::DATA_LENGTH = 11;
const int TShirtCannonPayload::PAYLOAD_LENGTH_LAST_INDEX = TShirtCannonPayload::PAYLOAD_LENGTH - 1;
const int TShirtCannonPayload::DATA_LENGTH_LAST_INDEX = TShirtCannonPayload::DATA_LENGTH - 1;

TShirtCannonPayload::TShirtCannonPayload()
{
    m_messageType = 0;
    m_messageIndex = 0;
    m_status = 0;
    m_error = 0;
    m_firingTime = 0;
    m_controllerDriveLeft = 0;
    m_controllerDriveRight = 0;
    m_batteryVoltage = 0;
    m_angle = 0;
    m_tankPressure = 0;
    m_firingPressure = 0;
}

bool TShirtCannonPayload::buildTransmission(uint8_t *transmission, const uint8_t len)
{
    if (len < PAYLOAD_LENGTH)
    {
        Serial.println("Message is not long enough to hold data.");
        return false;
    }

    uint8_t data[DATA_LENGTH];
    data[0] = m_messageType;
    data[1] = m_status;
    data[2] = m_error;
    data[3] = m_firingTime;
    data[4] = m_controllerDriveLeft;
    data[5] = m_controllerDriveRight;
    data[6] = m_batteryVoltage;
    data[7] = m_angle;
    data[8] = m_tankPressure;
    data[9] = m_firingPressure;
    data[10] = m_messageIndex;

    int bitPos = 0;
    int elem = PAYLOAD_LENGTH_LAST_INDEX;
    for (int i = DATA_LENGTH_LAST_INDEX; i >= 0; i--)
    {
        uint8_t attrVal = data[i];

        for (int j = 0; j < getAttributeSize(static_cast<AttributeSize>(i)); j++)
        {
            bitWrite(transmission[elem], bitPos, attrVal & 1);

            bitPos++;
            attrVal >>= 1;
            if (bitPos >= 8)
            {
                elem--;
                bitPos = 0;
            }
        }
    }
    return true;
}

bool TShirtCannonPayload::readMessage(const uint8_t *message, const uint8_t len)
{
    uint8_t data[DATA_LENGTH];
    memset(data, 0, sizeof(data));

    if (len < PAYLOAD_LENGTH)
    {
        Serial.println("Message is not long enough to fill in data.");
        return false;
    }

    int bitPos = 0;
    int elem = PAYLOAD_LENGTH_LAST_INDEX;

    uint8_t attrVal = message[PAYLOAD_LENGTH_LAST_INDEX];
    for (int i = DATA_LENGTH_LAST_INDEX; i >= 0; i--)
    {
        for (int j = 0; j < getAttributeSize(static_cast<AttributeSize>(i)); j++)
        {
            bitWrite(data[i], j, attrVal & 1);
            attrVal >>= 1;
            bitPos++;
            if (bitPos >= 8)
            {
                bitPos = 0;
                elem--;
                attrVal = message[elem];
            }
        }
    }

    // Make sure only relevant information is updated
    m_messageType = data[0];
    switch (m_messageType)
    {
    case 0:
        m_batteryVoltage = data[6];
        m_tankPressure = data[8];
        m_status = data[1] == 3 ? data[1] : m_status;
        break;
    case 1:
        m_messageIndex = data[10];
        m_status = data[1];
        m_firingTime = data[3];
        m_controllerDriveLeft = data[4];
        m_controllerDriveRight = data[5];
        m_angle = data[7];
        m_firingPressure = data[9];
        break;
    }
    m_error = data[2];

    return true;
}

uint8_t TShirtCannonPayload::getAttributeSize(AttributeSize attr)
{
    switch (attr)
    {
    // Comments after cases are possible reductions in size
    case AttributeSize::MESSAGE_TYPE: // -1
    case AttributeSize::BATTERY_VOLTAGE:
    case AttributeSize::ERROR:
        return 2;
    case AttributeSize::STATUS:
        return 3;
    case AttributeSize::MESSAGE_INDEX:
    case AttributeSize::FIRING_TIME: // -1
        return 5;
    case AttributeSize::ANGLE:           // -5
    case AttributeSize::TANK_PRESSURE:   // -2
    case AttributeSize::FIRING_PRESSURE: // -5 | -7
        return 7;
    case AttributeSize::CONTROLLER_DRIVE_LEFT:  // -1
    case AttributeSize::CONTROLLER_DRIVE_RIGHT: // -1
        return 8;
    default:
        return 0;
    }
}

void TShirtCannonPayload::print()
{
    Serial.println();
    Serial.print("Message Type: ");
    Serial.println(m_messageType, BIN);

    Serial.print("Message Index: ");
    Serial.println(m_messageIndex, BIN);

    Serial.print("Status: ");
    Serial.println(m_status, BIN);

    Serial.print("Error: ");
    Serial.println(m_error, BIN);

    Serial.print("Firing Time: ");
    Serial.println(m_firingTime, BIN);

    Serial.print("Left Drive: ");
    Serial.println(m_controllerDriveLeft, BIN);

    Serial.print("Right Drive: ");
    Serial.println(m_controllerDriveRight, BIN);

    Serial.print("Battery Voltage: ");
    Serial.println(m_batteryVoltage, BIN);

    Serial.print("Angle: ");
    Serial.println(m_angle, BIN);

    Serial.print("Tank Pressure: ");
    Serial.println(m_tankPressure, BIN);

    Serial.print("Firing Pressure: ");
    Serial.println(m_firingPressure, BIN);

    Serial.println();
}

// Getters
uint8_t TShirtCannonPayload::getMessageType()
{
    return this->m_messageType;
}

uint8_t TShirtCannonPayload::getMessageIndex()
{
    return this->m_messageIndex;
}

uint8_t TShirtCannonPayload::getStatus()
{
    return this->m_status;
}

uint8_t TShirtCannonPayload::getError()
{
    return this->m_error;
}

uint8_t TShirtCannonPayload::getFiringTime()
{
    return this->m_firingTime;
}

uint8_t TShirtCannonPayload::getControllerDriveLeft()
{
    return this->m_controllerDriveLeft;
}

uint8_t TShirtCannonPayload::getControllerDriveRight()
{
    return this->m_controllerDriveRight;
}

uint8_t TShirtCannonPayload::getBatteryVoltage()
{
    return this->m_batteryVoltage;
}

uint8_t TShirtCannonPayload::getAngle()
{
    return this->m_angle;
}

uint8_t TShirtCannonPayload::getTankPressure()
{
    return this->m_tankPressure;
}

uint8_t TShirtCannonPayload::getFiringPressure()
{
    return this->m_firingPressure;
}

// Setters
void TShirtCannonPayload::setMessageType(uint8_t value)
{
    this->m_messageType = value;
}

void TShirtCannonPayload::setMessageIndex(uint8_t value)
{
    this->m_messageIndex = value;
}

void TShirtCannonPayload::setStatus(uint8_t value)
{
    this->m_status = value;
}

void TShirtCannonPayload::setError(uint8_t value)
{
    this->m_error = value;
}

void TShirtCannonPayload::setFiringTime(uint8_t value)
{
    this->m_firingTime = value;
}

void TShirtCannonPayload::setControllerDriveLeft(uint8_t value)
{
    this->m_controllerDriveLeft = value;
}

void TShirtCannonPayload::setControllerDriveRight(uint8_t value)
{
    this->m_controllerDriveRight = value;
}

void TShirtCannonPayload::setBatteryVoltage(uint8_t value)
{
    this->m_batteryVoltage = value;
}

void TShirtCannonPayload::setAngle(uint8_t value)
{
    this->m_angle = value;
}

void TShirtCannonPayload::setTankPressure(uint8_t value)
{
    this->m_tankPressure = value;
}

void TShirtCannonPayload::setFiringPressure(uint8_t value)
{
    this->m_firingPressure = value;
}
