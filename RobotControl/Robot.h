#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include <TShirtCannonPayload.h>
#include <CommsI2CMaster.h>
#include "StatusLEDs.h"
#include "Utils.h"
#include <Servo.h>
#include "LinearActuator.h"

#define ROBOT_TICK_DURATION_BUFFER_LEN 5
#define PAYLOAD_LEN 7
#define SERIAL_BUFFER_LEN 50

class Robot
{
  static const unsigned long TICK_DURATION_MILLIS;
  static const uint8_t PREAMBLE_LEN;
  static const unsigned int KEEP_ALIVE_MILLIS;

  static const uint8_t STATUS_DISABLED;
  static const uint8_t STATUS_ADJUSTING;
  static const uint8_t STATUS_ENABLED;
  static const uint8_t STATUS_PRIMED;
  static const uint8_t STATUS_FIRING;

  static const uint8_t MAX_PAYLOAD_FIRING_VALUE;
  static const int MIN_FIRE_TIME_MILLIS;
  static const int PAYLOAD_TO_MILLIS;

public:
  Robot(TShirtCannonPayload &payload, int pinLedBuiltin, int i2cHostAddress, int i2cDeviceAddress, int fireSolenoidPin,
        int leftDrivePin, int rightDrivePin, int in1, int in2);

  void init();
  void update();

private:
  void updateSerial();
  void updatePayload(const uint8_t *data, const uint8_t len);
  void setRobot();
  void setStatus();
  int getAverageTickDuration();
  void updateTickDurations(int tickDurationMicros);
  int binToPWM(uint8_t value);
  void setError(const char *format, ...);

  TShirtCannonPayload &m_payload;
  StatusLEDs m_statusLEDs;
  CommsI2CMaster m_commsI2C;
  int m_initTimeSeconds;
  int m_tickDurations[ROBOT_TICK_DURATION_BUFFER_LEN];
  size_t m_tickDurationsIndex;
  uint8_t m_lastRecvIndex;
  unsigned long m_lastRecvTimeMillis;

  uint8_t m_payloadBytes[PAYLOAD_LEN];
  uint8_t m_serialBuffer[SERIAL_BUFFER_LEN];

  int m_fireSolenoidPin;

  bool m_firing;
  bool m_isHoldingFire;
  unsigned long m_fireTimeMillis;
  unsigned long m_solenoidCloseMillis;

  int m_leftDrivePin;
  int m_rightDrivePin;
  Servo m_leftDriveMotor;
  Servo m_rightDriveMotor;

  LinearActuator m_actuator;
};

#endif // ROBOT_H