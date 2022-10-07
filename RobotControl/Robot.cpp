#include "Robot.h"

const unsigned long Robot::TICK_DURATION_MILLIS = 100;
const uint8_t Robot::PREAMBLE_LEN = 4;
const unsigned int Robot::KEEP_ALIVE_MILLIS = 1000;

const uint8_t Robot::MAX_PAYLOAD_FIRING_VALUE = 20;
const int Robot::MIN_FIRE_TIME_MILLIS = 100;
const int Robot::PAYLOAD_TO_MILLIS = 10;

void RobotStatus::setRobot(Robot *robot) {
    m_robot = robot;
}

Robot::Robot(TShirtCannonPayload &payload, int pinLedBuiltin, int i2cHostAddress, int i2cDeviceAddress,
  int fireSolenoidPin, int in1, int in2,
  StatusDisabled &disabled, StatusEnabled &enabled, StatusAdjusting &adjusting, StatusPrimed &primed, StatusFiring &firing) :
  m_payload(payload),
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

  m_currentStatus = STATUS_DISABLED;
  m_statuses[STATUS_DISABLED] = &disabled;
  m_statuses[STATUS_ENABLED] = &enabled;
  m_statuses[STATUS_ADJUSTING] = &adjusting;
  m_statuses[STATUS_PRIMED] = &primed;
  m_statuses[STATUS_FIRING] = &firing;

  for(int i = 0; i < 5; i++) {
    m_statuses[i]->setRobot(this);
  }
}

void Robot::init() {
  m_initTimeSeconds = (int)(millis() / 1000);

  m_tickDurationsIndex = 0;
  for (int i = 0; i < ROBOT_TICK_DURATION_BUFFER_LEN; i++) {
    m_tickDurations[i] = 0;
  }

  pinMode(m_fireSolenoidPin, OUTPUT);
  digitalWrite(m_fireSolenoidPin, LOW);

  m_statusLEDs.setBlinkPattern(StatusLEDs::DISABLED);
  m_commsI2C.init();
  m_actuator.init();
}

void Robot::update() {
  unsigned long tickStartMillis = millis();
  int tickDurationMillis = millis() - tickStartMillis;

  int tick = m_payload.getMessageIndex();

  m_statusLEDs.update(tick);

  // TODO: Remove after timing is solved
  updateTickDurations(tickDurationMillis);

  updateSerial();

  setFireTime();
  
  transition(static_cast<Status>(m_payload.getStatus()));

  m_statuses[m_currentStatus]->update();

  if (tickDurationMillis > TICK_DURATION_MILLIS) {
    //setError("Tick %d ms", tickDurationMillis);
  }
}

void Robot::transition(Status status) {
  m_currentStatus = status;
  m_payload.setStatus(status);
  m_statuses[m_currentStatus]->validateState();
}

void Robot::updateSerial() {
  memset(m_payloadBytes, 0, PAYLOAD_LEN);
  memset(m_serialBuffer, 0, SERIAL_BUFFER_LEN);

  if (m_commsI2C.getBytes(m_serialBuffer, SERIAL_BUFFER_LEN, m_payloadBytes, PAYLOAD_LEN)) {
    updatePayload(m_payloadBytes, PAYLOAD_LEN);
  }
}

void Robot::updatePayload(const uint8_t *data, const uint8_t len) {
  bool success = m_payload.readMessage(data, len);

  const uint8_t status = m_payload.getStatus();
  const uint8_t err = m_payload.getError();

  if (err > 0 || !success) {
    m_statusLEDs.setBlinkPattern(StatusLEDs::ERROR);
  } else {
    if (status == STATUS_DISABLED) {
      m_statusLEDs.setBlinkPattern(StatusLEDs::DISABLED);
    } else if (status == STATUS_ENABLED) {
      m_statusLEDs.setBlinkPattern(StatusLEDs::ENABLED);
    } else if (status == STATUS_PRIMED) {
      m_statusLEDs.setBlinkPattern(StatusLEDs::PRIMED);
    } else {
      m_statusLEDs.setBlinkPattern(StatusLEDs::OFF);
    }
  }
}

void Robot::setFireTime() {
  uint8_t vlvTime = m_payload.getFiringTime();
  if(vlvTime > MAX_PAYLOAD_FIRING_VALUE) {
    vlvTime = 0;
  } else {
    m_fireTimeMillis = MIN_FIRE_TIME_MILLIS + (vlvTime * PAYLOAD_TO_MILLIS);
  }
}

void Robot::stopDriving() {
  Serial.write((uint8_t)0);
  Serial.write((uint8_t)128);
}

void Robot::setDrive() {
  Serial.write(m_payload.getControllerDriveLeft());
  Serial.write(m_payload.getControllerDriveRight());
}

void Robot::fire() {
  if (!m_isHoldingFire) {
    digitalWrite(m_fireSolenoidPin, HIGH);
    m_solenoidCloseMillis = millis() + m_fireTimeMillis;
    m_firing = true;
    m_isHoldingFire = true;
    transition(STATUS_ADJUSTING);
  }
}

void Robot::stopFiring() {
  digitalWrite(m_fireSolenoidPin, LOW);
  m_firing = false;
}

void Robot::handleFiring() {
  if (m_firing) {
    transition(STATUS_ADJUSTING);
  }
}

void Robot::handleAngle() {
  if(m_actuator.isMoving(m_payload.getAngle())) {
    transition(STATUS_ADJUSTING);
  } 
}

void Robot::keepAlive() {
  int currentIndex = m_payload.getMessageIndex();
  if (m_lastRecvIndex != currentIndex) {
    m_lastRecvTimeMillis = millis();
    m_lastRecvIndex = currentIndex;
  }
  if(millis() - m_lastRecvTimeMillis > KEEP_ALIVE_MILLIS) {
    m_payload.setStatus(STATUS_DISABLED);
  }
}

int Robot::getAverageTickDuration() {
  unsigned long total = 0;
  for (int i = 0; i < ROBOT_TICK_DURATION_BUFFER_LEN; i++) {
    total += m_tickDurations[i];
  }
  return (int)(total / ROBOT_TICK_DURATION_BUFFER_LEN);
}

void Robot::updateTickDurations(int tickDurationMillis) {
  m_tickDurations[m_tickDurationsIndex] = tickDurationMillis;
  m_tickDurationsIndex = Utils::incrementRingBufferIndex(m_tickDurationsIndex, ROBOT_TICK_DURATION_BUFFER_LEN);
}

void Robot::setError(const char *format, ...) {
  va_list args;
  char message[32];
  va_start(args, format);
  vsprintf(message, format, args);
  va_end(args);

  m_payload.setStatus(STATUS_DISABLED);
}

void StatusDisabled::validateState() {
  return;
}

void StatusDisabled::update() {
  m_robot->stopDriving();
  m_robot->stopFiring();
  m_robot->m_isHoldingFire = false;
  m_robot->m_actuator.stop();
}

void StatusEnabled::validateState() {
  m_robot->keepAlive();
  m_robot->handleFiring();
  m_robot->handleAngle();
}

void StatusEnabled::update() {
  m_robot->setDrive();
  m_robot->m_isHoldingFire = false;
}

void StatusAdjusting::validateState() {
  m_robot->keepAlive();
}

void StatusAdjusting::update() {
  m_robot->stopDriving();
  if(m_robot->m_firing && millis() >= m_robot->m_solenoidCloseMillis) {
    m_robot->stopFiring();
  }
  m_robot->m_actuator.update(m_robot->m_payload.getAngle());
}

void StatusPrimed::validateState() {
  m_robot->keepAlive();
  m_robot->handleFiring();
}

void StatusPrimed::update() {
  m_robot->stopDriving();
  m_robot->m_isHoldingFire = false;
}

void StatusFiring::validateState() {
  m_robot->keepAlive();
  m_robot->handleFiring();
}

void StatusFiring::update() {
  m_robot->stopDriving();
  m_robot->fire();
}