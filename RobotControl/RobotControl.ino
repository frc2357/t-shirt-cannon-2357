/*
 * RobotControl.ino
 *
 * This is the main control program for the robot controller.
 * It can receive control information via i2c and then
 * controls the individual motors, solenoids, and devices.
 */

#include <TShirtCannonPayload.h>
#include "Utils.h"
#include "Robot.h"
#include "StatusLEDs.h"

#define SOFTWARE_VERSION 1
#define SERIAL_BAUD_RATE 115200
#define I2C_HOST_ADDRESS 0x09
#define I2C_DEVICE_ADDRESS 0x08
#define PIN_LED_BUILTIN LED_BUILTIN
#define FIRE_SOLENOID_PIN 3
// #define ANGLE_IN1 10
// #define ANGLE_IN2 9

// 0.00067 = in/per
// 8,955.22388 = milliseconds to max travel

// 455 milliseconds of buffer or 0.3 inches

#define LEFT_DRIVE_PWM 9
#define RIGHT_DRIVE_PWM 10

// Set up the JSON State for the robot
TShirtCannonPayload payload;

Robot robot(payload, PIN_LED_BUILTIN, I2C_HOST_ADDRESS, I2C_DEVICE_ADDRESS, FIRE_SOLENOID_PIN, LEFT_DRIVE_PWM, RIGHT_DRIVE_PWM);

// Primary Setup
void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  if (Serial)
  {
    Serial.print("----------- RobotControl v");
    Serial.print(SOFTWARE_VERSION);
    Serial.println(" -----------");
  }
  robot.init();
  Serial.print("----------- Init complete -----------");
}

// Main Loop
void loop()
{
  robot.update();
}
