#ifndef ROBOTCOMMS_H
#define ROBOTCOMMS_H

#include <Arduino.h>
#include <SPI.h>
#include <RFM95C.h>
#include <CommsI2CSlave.h>
#include <TShirtCannonPayload.h>

#define PAYLOAD_LEN 7

class RobotComms
{
  static const uint8_t PREAMBLE_LEN;

public:
  RobotComms(unsigned int radioSS, unsigned int radioINT, unsigned int i2cHostAddress);
  void init(unsigned int radioFreq, void (*recFunction)(int), void (*reqFunction)(void));
  void update();

  void onI2CReceive(int bytesRead);
  void onI2CRequest();

private:
  RFM_95C m_radio;
  CommsI2CSlave m_commsI2C;
  uint8_t m_payload[PAYLOAD_LEN];
};

#endif