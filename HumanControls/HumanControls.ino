#include <LiquidCrystal_I2C.h>
#include <JsonElement.h>
#include "RotaryKnobController.h"
#include "MenuController.h"
#include "DisplayController.h"
#include "Page.h"
#include "JoystickAxis.h"
#include "EnableController.h"
#include "FireController.h"
#include "HumanControls.h"
#include "TShirtCannonPayload.h"

// Version
#define SOFTWARE_VERSION 1

// Pins
#define JOYSTICK_PIN_VRX A0 // Analog Pin for joystick x
#define JOYSTICK_PIN_VRY A1 // Analog Pin for joystick y
#define ENCODER_PIN_SW 13   // Gets the button for rotary knob
#define ENCODER_PIN_A 5     // CLK gets degrees for rotary knob
#define ENCODER_PIN_B 6     // DT gets direction for rotary knob
#define ENABLE_PIN 10       // Digital Pin for the enable button
#define PRIME_PIN 11        // Digital Pin for prime button
#define FIRE_PIN 12         // Digital Pin for the fire button
#define RFM95_CS PIN_RFM_CS
#define RFM95_INT PIN_RFM_DIO0
#define RFM95_RST PIN_RFM_RST

// Other constraints
#define NUM_BUTTONS 3             // Number of buttons to give the debouncer
#define HANG_TIMER_DURATION 20000 // Amount in milliseconds to stay on a page before going to dash
#define USB_BAUDRATE 115200
#define ROBOT_BATTERY_CHAR 2      // Custom char code for robot battery bar
#define CONTROLLER_BATTERY_CHAR 3 // Custom char code for controller battery bar
#define RFM95_FREQ 915.0
#define RFM95_TX_POWER 23
#define PAYLOAD_LENGTH 7
#define WAIT_FOR_CONTROLS_DATA_DELAY_MILLIS 5
#define WAIT_FOR_ROBOT_MESSAGE_DELAY_MILLIS 5

// Min - Max
#define ANGLE_INCREMENT 1    // Increment amount for elevator angle
#define ANGLE_MIN 20         // Minimum elevator angle
#define ANGLE_MAX 70         // Maximum elevator angle
#define PRESSURE_INCREMENT 1 // Increment amount for shot pressure
#define PRESSURE_MIN 60      // Minimum shot pressure
#define PRESSURE_MAX 120     // Maximum shot pressure
#define DURATION_INCREMENT 1 // Increment amount for valve duration
#define DURATION_MIN 0       // Minimum valve duration (100 ms)
#define DURATION_MAX 15      // Maximum valve duration (250 ms)

// Joystick deadzones and max
#define X_DEAD_ZONE_SIZE 25 // Total size of the X deadzone
#define JOYSTICK_MAX 1023   // Maximum joystick value that comes from the sensor
#define Y_DEAD_ZONE_SIZE 25 // Total size of the y deadzone

// Global communication variables
boolean hasRobotMessage = false;
boolean hasRobotDataChanged = false;
boolean hasControllerMessage = false;
boolean hasControllerDataChanged = false;

uint8_t currentControllerMessage[7];
uint8_t lastControllerMessage[7];
uint8_t currentRobotMessage[7];
uint8_t lastRobotMessage[7];

// Create payload object
TShirtCannonPayload payload;

CommunicationDriver commDriver(RFM95_FREQ, RFM95_TX_POWER, PAYLOAD_LENGTH, RFM95_CS, RFM95_INT, RFM95_RST);
HumanControls humanControls(payload, ENCODER_PIN_A, ENCODER_PIN_B, ANGLE_INCREMENT, ANGLE_MIN, ANGLE_MAX,
                            PRESSURE_INCREMENT, PRESSURE_MIN, PRESSURE_MAX, DURATION_INCREMENT, DURATION_MIN,
                            DURATION_MAX, HANG_TIMER_DURATION, NUM_BUTTONS, ENCODER_PIN_SW, ENABLE_PIN,
                            PRIME_PIN, FIRE_PIN, JOYSTICK_PIN_VRX, X_DEAD_ZONE_SIZE, JOYSTICK_MAX,
                            JOYSTICK_PIN_VRY, Y_DEAD_ZONE_SIZE);

// Comms core
void setup()
{
  Serial.begin(USB_BAUDRATE);
  commDriver.connect(payload);
}

void loop()
{
  if (hasControllerMessage)
  {
    commDriver.sendControllerMessage(currentControllerMessage);
    hasControllerMessage = false;
    delay(WAIT_FOR_CONTROLS_DATA_DELAY_MILLIS);
  }
  if (hasRobotMessage)
  {
    // TODO: log error. Control loop taking too long with robot message
  }
  delay(WAIT_FOR_ROBOT_MESSAGE_DELAY_MILLIS);
  hasRobotMessage = commDriver.recvRobotMessage(currentRobotMessage);
}

// Controls core
void setup1()
{
  humanControls.init();
}

void loop1()
{
  if (hasRobotMessage)
  {
    if (memcmp(currentRobotMessage, lastRobotMessage, PAYLOAD_LENGTH) != 0)
    {
      hasRobotDataChanged = true;
    }
    memcpy(lastRobotMessage, currentRobotMessage, PAYLOAD_LENGTH);
    hasRobotMessage = false;
    if (hasRobotDataChanged)
    {
      payload.readMessage(currentRobotMessage, PAYLOAD_LENGTH);
      hasRobotDataChanged = false;
    }
  }
  humanControls.update(currentControllerMessage);
  if (!hasControllerMessage)
  {
    hasControllerMessage = true;
    memcpy(lastControllerMessage, currentControllerMessage, PAYLOAD_LENGTH);
  }
}

void onPinActivated(int pinNr)
{
  humanControls.onPinActivated(pinNr);
}

void onPinDeactivated(int pinNr)
{
  humanControls.onPinDeactivated(pinNr);
}