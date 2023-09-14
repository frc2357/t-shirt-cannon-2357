#include "RobotComms.h"

const uint8_t RobotComms::PREAMBLE_LEN = 4;

RobotComms::RobotComms(unsigned int radioSS, unsigned int radioINT, unsigned int i2cHostAddress)
    : m_radio(radioSS, radioINT), m_commsI2C(i2cHostAddress, PREAMBLE_LEN, m_payload, PAYLOAD_LEN)
{
}

void RobotComms::init(unsigned int radioFreq, void (*recFunction)(int), void (*reqFunction)(void))
{
    if (!m_radio.init())
        Serial.println("radio init failed");

    if (!m_radio.setFrequency(radioFreq))
    {
        Serial.println("radio setFrequency failed");
        while (1)
            ;
    }

    m_radio.setTxPower(23, false);

    m_commsI2C.init(recFunction, reqFunction);

    Serial.println("Finished init");
}

void RobotComms::update()
{
    uint8_t payloadLen = PAYLOAD_LEN;

    if (m_radio.recv(m_payload, &payloadLen))
    {
        Serial.println("Radio message received");
    }
    else
    {
        Serial.println("No radio message");
    }
}

void RobotComms::onI2CReceive(int bytesRead)
{
    m_commsI2C.getBytes();
}
void RobotComms::onI2CRequest()
{
    m_commsI2C.sendBytes();
    Serial.println("Data requested");
}
