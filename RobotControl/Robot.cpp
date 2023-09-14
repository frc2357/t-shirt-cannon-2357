#include "Robot.h"

const unsigned long Robot::TICK_DURATION_MILLIS = 100;
const uint8_t Robot::PREAMBLE_LEN = 4;
const unsigned int Robot::KEEP_ALIVE_MILLIS = 1000;

const uint8_t Robot::STATUS_DISABLED = 1;
const uint8_t Robot::STATUS_ENABLED = 2;
const uint8_t Robot::STATUS_ADJUSTING = 3;
const uint8_t Robot::STATUS_PRIMED = 4;
const uint8_t Robot::STATUS_FIRING = 5;

const uint8_t Robot::MAX_PAYLOAD_FIRING_VALUE = 20;
const int Robot::MIN_FIRE_TIME_MILLIS = 100;
const int Robot::PAYLOAD_TO_MILLIS = 10;

Robot::Robot(TShirtCannonPayload &payload, int pinLedBuiltin, int i2cHostAddress, int i2cDeviceAddress,
             int fireSolenoidPin, int leftDrivePin, int rightDrivePin, int in1, int in2)
    : m_payload(payload),
      m_statusLEDs(pinLedBuiltin),
      m_commsI2C(i2cHostAddress, i2cDeviceAddress, PREAMBLE_LEN),
      m_actuator(in1, in2)
{
  m_initTimeSeconds = 0;
  m_lastRecvIndex = -1;
  m_lastRecvTimeMillis = 0;
  m_fireSolenoidPin = fireSolenoidPin;
  m_firing = false;
  m_isHoldingFire = false;

  m_fireTimeMillis = 100;

  m_leftDrivePin = leftDrivePin;
  m_rightDrivePin = rightDrivePin;
}

void Robot::init()
{
  m_initTimeSeconds = (int)(millis() / 1000);

  m_tickDurationsIndex = 0;
  for (int i = 0; i < ROBOT_TICK_DURATION_BUFFER_LEN; i++)
  {
    m_tickDurations[i] = 0;
  }

  pinMode(m_fireSolenoidPin, OUTPUT);
  digitalWrite(m_fireSolenoidPin, LOW);

  m_leftDriveMotor.attach(m_leftDrivePin, 1000, 2000);
  m_rightDriveMotor.attach(m_rightDrivePin, 1000, 2000);

  m_statusLEDs.setBlinkPattern(StatusLEDs::DISABLED);
  m_commsI2C.init();
}

void Robot::update()
{
  unsigned long tickStartMillis = millis();
  int tickDurationMillis = millis() - tickStartMillis;

  int tick = m_payload.getMessageIndex();

  m_statusLEDs.update(tick);

  // TODO: Remove after timing is solved
  // Serial.print("Tick time: ");
  // Serial.println(tickDurationMillis);
  updateTickDurations(tickDurationMillis);

  updateSerial();

  if (tickDurationMillis > TICK_DURATION_MILLIS)
  {
    setError("Tick %d ms", tickDurationMillis);
  }

  int timeLeftMillis = TICK_DURATION_MILLIS - (millis() - tickStartMillis);
  if (timeLeftMillis > 0)
  {
    // delay(timeLeftMillis);
  }
}

void Robot::updateSerial()
{
  memset(m_payloadBytes, 0, PAYLOAD_LEN);
  memset(m_serialBuffer, 0, SERIAL_BUFFER_LEN);

  if (m_commsI2C.getBytes(m_serialBuffer, SERIAL_BUFFER_LEN, m_payloadBytes, PAYLOAD_LEN))
  {
    updatePayload(m_payloadBytes, PAYLOAD_LEN);
    // Serial.println("Bytes parsed");
  }

  setStatus();
  setRobot();
}

void Robot::updatePayload(const uint8_t *data, const uint8_t len)
{
  bool success = m_payload.readMessage(data, len);

  const uint8_t status = m_payload.getStatus();
  const uint8_t err = m_payload.getError();

  if (err > 0 || !success)
  {
    m_statusLEDs.setBlinkPattern(StatusLEDs::ERROR);
    m_payload.setStatus(STATUS_DISABLED);
  }
  else
  {
    if (status == STATUS_DISABLED)
    {
      m_statusLEDs.setBlinkPattern(StatusLEDs::DISABLED);
    }
    else if (status == STATUS_ENABLED)
    {
      m_statusLEDs.setBlinkPattern(StatusLEDs::ENABLED);
    }
    else if (status == STATUS_PRIMED)
    {
      m_statusLEDs.setBlinkPattern(StatusLEDs::PRIMED);
    }
    else
    {
      m_statusLEDs.setBlinkPattern(StatusLEDs::OFF);
    }
  }
}

void Robot::setRobot()
{
  uint8_t status = m_payload.getStatus();

  uint8_t vlvTime = m_payload.getFiringTime();

  if (vlvTime > MAX_PAYLOAD_FIRING_VALUE)
  {
    vlvTime = 0;
  }
  else
  {
    m_fireTimeMillis = MIN_FIRE_TIME_MILLIS + (vlvTime * PAYLOAD_TO_MILLIS);
  }

  if (m_firing && millis() >= m_solenoidCloseMillis)
  {
    digitalWrite(m_fireSolenoidPin, LOW);
    m_firing = false;
  }

  if (status != STATUS_ENABLED)
  {
    m_leftDriveMotor.write(90);
    m_rightDriveMotor.write(90);
  }

  if (status != STATUS_FIRING && status != STATUS_ADJUSTING)
  {
    digitalWrite(m_fireSolenoidPin, LOW);
    m_firing = false;
    m_isHoldingFire = false;
  }

  if (status == STATUS_ENABLED)
  {
    m_leftDriveMotor.write(binToPWM(m_payload.getControllerDriveLeft()));
    m_rightDriveMotor.write(binToPWM(m_payload.getControllerDriveRight()));
  }

  if (status == STATUS_FIRING)
  {
    if (!m_isHoldingFire)
    {
      digitalWrite(m_fireSolenoidPin, HIGH);
      status = STATUS_ADJUSTING;
      m_solenoidCloseMillis = millis() + m_fireTimeMillis;
      m_firing = true;
      m_isHoldingFire = true;
    }
  }
  m_payload.setStatus(status);
}

void Robot::setStatus()
{
  // First check if status should be Adjusting
  if (m_payload.getStatus() == STATUS_DISABLED)
  {
    return;
  }

  if (m_firing && millis() < m_solenoidCloseMillis)
  {
    m_payload.setStatus(STATUS_ADJUSTING);
  }

  // Second check if status should be Disabled
  int currentIndex = m_payload.getMessageIndex();
  if (m_lastRecvIndex != currentIndex)
  {
    m_lastRecvTimeMillis = millis();
    m_lastRecvIndex = currentIndex;
  }

  if (millis() - m_lastRecvTimeMillis > KEEP_ALIVE_MILLIS)
  {
    m_payload.setStatus(STATUS_DISABLED);
  }
}

int Robot::getAverageTickDuration()
{
  unsigned long total = 0;
  for (int i = 0; i < ROBOT_TICK_DURATION_BUFFER_LEN; i++)
  {
    total += m_tickDurations[i];
  }
  return (int)(total / ROBOT_TICK_DURATION_BUFFER_LEN);
}

void Robot::updateTickDurations(int tickDurationMillis)
{
  m_tickDurations[m_tickDurationsIndex] = tickDurationMillis;
  m_tickDurationsIndex = Utils::incrementRingBufferIndex(m_tickDurationsIndex, ROBOT_TICK_DURATION_BUFFER_LEN);
}

int Robot::binToPWM(uint8_t value)
{
  int dir = value & 64;

  int speed = value & 63;

  int mappedSpeed = map(speed, 0, 63, 0, 90);

  return 90 + (dir == 64 ? -mappedSpeed : mappedSpeed);
}

void Robot::setError(const char *format, ...)
{
  va_list args;
  char message[32];
  va_start(args, format);
  vsprintf(message, format, args);
  va_end(args);

  m_payload.setStatus(STATUS_DISABLED);

  // Serial.print("ERROR: ");
  // Serial.println(message);
}