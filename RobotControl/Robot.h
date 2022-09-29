#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include <TShirtCannonPayload.h>
#include <CommsI2CMaster.h>
#include "StatusLEDs.h"
#include "Utils.h"
#include "LinearActuator.h"

#define ROBOT_TICK_DURATION_BUFFER_LEN 5
#define PAYLOAD_LEN 7
#define SERIAL_BUFFER_LEN 50
#define NUM_STATUSES 5

enum Status {STATUS_DISABLED = 0, STATUS_ENABLED = 1, STATUS_ADJUSTING = 2, STATUS_PRIMED = 3, STATUS_FIRING = 4};

class Robot;

class RobotStatus {
public:
    void setRobot(Robot *robot);
    virtual void update(); // What to do to the robot every loop
    virtual void validateState(); // Checks validity of called state, method will transition to another if necessary

protected:
    Robot *m_robot;
};

class StatusDisabled;
class StatusEnabled;
class StatusAdjusting;
class StatusPrimed;
class StatusFiring;

class Robot
{
  static const unsigned long TICK_DURATION_MILLIS;
  static const uint8_t PREAMBLE_LEN;
  static const unsigned int KEEP_ALIVE_MILLIS;

  static const unsigned long TEMP_FIRE_TIME_MILLIS;

  static const uint8_t MAX_PAYLOAD_FIRING_VALUE;
  static const int MIN_FIRE_TIME_MILLIS;
  static const int PAYLOAD_TO_MILLIS;

  friend StatusDisabled;
  friend StatusEnabled;
  friend StatusAdjusting;
  friend StatusPrimed;
  friend StatusFiring;

public:
  Robot(TShirtCannonPayload &payload, int pinLedBuiltin, int i2cHostAddress, int i2cDeviceAddress, int fireSolenoidPin, int en, int in1, int in2, uint8_t speed,
    StatusDisabled &disabled, StatusEnabled &enabled, StatusAdjusting &adjusting, StatusPrimed &primed, StatusFiring &firing);

  void init();
  void update();
  void transition(Status status);

private:
  void updateSerial();
  void updatePayload(const uint8_t *data, const uint8_t len);
  int getAverageTickDuration();
  void updateTickDurations(int tickDurationMicros);
  void setError(const char *format, ...);
  void setFireTime();
  void setDrive();
  void stopDriving();
  void fire();
  void stopFiring();
  void keepAlive();
  void handleFiring();

  TShirtCannonPayload &m_payload;
  StatusLEDs m_statusLEDs;
  CommsI2CMaster m_commsI2C;
  LinearActuator m_actuator;
  
  int m_initTimeSeconds;
  int m_tickDurations[ROBOT_TICK_DURATION_BUFFER_LEN];
  size_t m_tickDurationsIndex;
  uint8_t m_lastRecvIndex;
  unsigned long m_lastRecvTimeMillis;

  uint8_t m_payloadBytes[PAYLOAD_LEN];
  uint8_t m_serialBuffer[SERIAL_BUFFER_LEN];

  int m_fireSolenoidPin;
  unsigned long m_solenoidOpenMillis;

  bool m_firing;
  bool m_isHoldingFire;
  unsigned long m_fireTimeMillis;
  unsigned long m_solenoidCloseMillis;

  RobotStatus *m_statuses[NUM_STATUSES];
  Status m_currentStatus;
};

class StatusDisabled : public RobotStatus {
public:
    void update();
    void validateState();
};

class StatusEnabled : public RobotStatus {
public:
    void update();
    void validateState();
};

class StatusAdjusting : public RobotStatus {
public:
    void update();
    void validateState();
};

class StatusPrimed : public RobotStatus {
public:
    void update();
    void validateState();
};

class StatusFiring : public RobotStatus {
public:
    void update();
    void validateState();
};

#endif // ROBOT_H