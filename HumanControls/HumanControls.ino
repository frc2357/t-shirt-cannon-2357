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
#include "LinkedList.h"

// Version
#define SOFTWARE_VERSION 1

// Pins
#define JOYSTICK_PIN_VRX 0 // Analog Pin for joystick x
#define JOYSTICK_PIN_VRY 1 // Analog Pin for joystick y
#define ENCODER_PIN_SW 13   // Gets the button for rotary knob
#define ENCODER_PIN_A 5  // CLK gets degrees for rotary knob
#define ENCODER_PIN_B 6   // DT gets direction for rotary knob
#define ENABLE_PIN 10      // Digital Pin for the enable button
#define PRIME_PIN 11       // Digital Pin for prime button
#define FIRE_PIN 12        // Digital Pin for the fire button
#define I2C_SDA 20         // I2C used by LCD
#define I2C_SCL 21         // I2C used by LCD
#define RFM95_CS 8
#define RFM95_INT 3


// Other constraints
#define DISPLAY_ADDRESS 0X27      // I2c address of the lcd display
#define DISPLAY_LENGTH 16         // Length of the lcd display
#define DISPLAY_WIDTH 2           // width of the lcd display
#define NUM_BUTTONS 3             // Number of buttons to give the debouncer
#define HANG_TIMER_DURATION 10000 // Amount in milliseconds to stay on a page before going to dash
#define USB_BAUDRATE 115200
#define ROBOT_BATTERY_CHAR 2      // Custom char code for robot battery bar
#define CONTROLLER_BATTERY_CHAR 3 // Custom char code for controller battery bar
#define RFM95_FREQ 915.0
#define RFM95_TX_POWER 23
#define LINKED_LIST_MAX_SIZE 128

// Min - Max
#define ANGLE_INCREMENT 1     // Increment amount for elevator angle
#define ANGLE_MIN 20          // Minimum elevator angle
#define ANGLE_MAX 70          // Maximum elevator angle
#define PRESSURE_INCREMENT 1  // Increment amount for shot pressure
#define PRESSURE_MIN 60       // Minimum shot pressure
#define PRESSURE_MAX 120      // Maximum shot pressure
#define DURATION_INCREMENT 10 // Increment amount for valve duration
#define DURATION_MIN 100      // Minimum valve duration
#define DURATION_MAX 250      // Maximum valve duration

// Joystick deadzones and max
#define X_DEAD_ZONE_SIZE 100 // Total size of the X deadzone
#define JOYSTICK_MAX 1023    // Maximum joystick value that comes from the sensor
#define Y_DEAD_ZONE_SIZE 100 // Total size of the y deadzone

// Create payload object
TShirtCannonPayload payload;
LinkedList messageQueue(LINKED_LIST_MAX_SIZE);

HumanControls humanControls(payload, messageQueue, ENCODER_PIN_A, ENCODER_PIN_B, ANGLE_INCREMENT, ANGLE_MIN, ANGLE_MAX,
                            PRESSURE_INCREMENT, PRESSURE_MIN, PRESSURE_MAX, DURATION_INCREMENT, DURATION_MIN,
                            DURATION_MAX, HANG_TIMER_DURATION, NUM_BUTTONS, ENCODER_PIN_SW, ENABLE_PIN, 
                            PRIME_PIN, FIRE_PIN, JOYSTICK_PIN_VRX, X_DEAD_ZONE_SIZE, JOYSTICK_MAX,
                            JOYSTICK_PIN_VRY, Y_DEAD_ZONE_SIZE, RFM95_CS, RFM95_INT, RFM95_FREQ, RFM95_TX_POWER);

void setup()
{
    Serial.begin(USB_BAUDRATE);
    humanControls.init();
}

void loop()
{
    humanControls.update();
}

void onPinActivated(int pinNr)
{
    humanControls.onPinActivated(pinNr);
}

void onPinDeactivated(int pinNr)
{
    humanControls.onPinDeactivated(pinNr);
}