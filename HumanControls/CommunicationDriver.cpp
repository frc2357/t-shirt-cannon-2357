#include "CommunicationDriver.h"

CommunicationDriver::CommunicationDriver(int freq, int txPower, uint8_t payloadLength, uint8_t rfm95_cs, uint8_t rfm95_int, uint8_t rfm95_rst)
    : m_driver(rfm95_cs, rfm95_int), m_payloadLength(payloadLength)
{
  this->m_rfm95_rst = rfm95_rst;
  this->m_rfm95Freq = freq;
  this->m_rfm95TxPower = txPower;
}

void CommunicationDriver::connect(TShirtCannonPayload &payload)
{
  pinMode(m_rfm95_rst, OUTPUT);
  digitalWrite(m_rfm95_rst, HIGH);

  if (!m_driver.init())
  {
    Serial.println("Driver init failed");
    return;
  }

  if (!m_driver.setFrequency(m_rfm95Freq))
  {
    Serial.println("Could not set driver frequency");
    return;
  }

  m_driver.setTxPower(m_rfm95TxPower, false);

  if (payload.getStatus() == Utils::ControllerStatus::DISCONNECTED)
  {
    payload.setStatus(Utils::ControllerStatus::DISABLED);
  }
}

void CommunicationDriver::sendControllerMessage(uint8_t *message)
{
  m_driver.send(message, m_payloadLength);
}

boolean CommunicationDriver::recvRobotMessage(uint8_t *messageBuffer)
{
  return m_driver.recv(messageBuffer, &m_payloadLength);
}