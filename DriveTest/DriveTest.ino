#include "JoystickAxis.h"
#include "Utils.h"
#include <TShirtCannonPayload.h>
#include "RFM95C.h"
#include "FTDebouncer.h"
#include "RotaryEncoder.h"
#include <SerLCD.h>
#include "RotaryKnobController.h"

// Pins
#define JOYSTICK_PIN_VRX 0
#define JOYSTICK_PIN_VRY 1
#define ENCODER_PIN_A 5
#define ENCODER_PIN_B 6
#define ENCODER_PIN_SW 13
#define ENABLE_PIN 10
#define PRIME_PIN 11
#define FIRE_PIN 12
#define POWER_DOWN_PIN 19

// Joystick variables
#define DEAD_ZONE_SIZE 100
#define JOYSTICK_MAX 1023

// RFM95 pins
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// RFM95 variables
#define RF95_FREQ 915.0

// Addresses
#define CONTROLLER_ADDRESS 1
#define ROBOT_ADDRESS 2

RFM_95C raw_driver(RFM95_CS, RFM95_INT);

FTDebouncer m_pinDebouncer(1);

TShirtCannonPayload payload;

JoystickAxis leftStick(JOYSTICK_PIN_VRY, DEAD_ZONE_SIZE, JOYSTICK_MAX);
JoystickAxis rightStick(JOYSTICK_PIN_VRX, DEAD_ZONE_SIZE, JOYSTICK_MAX);

uint8_t buf[7];

RotaryKnobController encoder(ENCODER_PIN_A, ENCODER_PIN_B);

SerLCD lcd;
char lcdText[33];
char strInt[8];
void setup()
{
    // ! UNCOMMENT THE LINES BELOW IF USING CONTROLLER
    pinMode(POWER_DOWN_PIN, OUTPUT);
    digitalWrite(POWER_DOWN_PIN, HIGH);

    Serial.begin(115200);

    Wire.begin();
    lcd.begin(Wire);
    lcd.disableSystemMessages();

    lcd.setBacklight(0, 0, 255);
    lcd.setContrast(5);

    if (!raw_driver.init())
    {
        Serial.println("Driver init failed");
    }

    if (!raw_driver.setFrequency(RF95_FREQ))
    {
        Serial.println("Could not set driver frequency");
        while (1)
            ;
    }

    raw_driver.setTxPower(23, false);

    m_pinDebouncer.addPin(ENCODER_PIN_SW, HIGH, INPUT_PULLUP);

    m_pinDebouncer.addPin(ENABLE_PIN, HIGH, INPUT_PULLUP);
    m_pinDebouncer.addPin(PRIME_PIN, HIGH, INPUT_PULLUP);
    m_pinDebouncer.addPin(FIRE_PIN, HIGH, INPUT_PULLUP);

    m_pinDebouncer.begin();
}

void loop()
{
    payload.setMessageIndex((payload.getMessageIndex() + 1) % 32);

    if (digitalRead(ENCODER_PIN_SW) == 0)
    {
        digitalWrite(POWER_DOWN_PIN, LOW);
    }

    rightStick.update();
    leftStick.update();
    m_pinDebouncer.update();

    float turn, speed;
    turn = rightStick.getResult();
    speed = leftStick.getResult();

    Utils::setMotors(payload, turn, speed);
    //Utils::setAngle(payload, encoder);3

    int dir = encoder.getValue();
    uint8_t time = payload.getFiringTime();

    memset(lcdText, ' ', 16 * 2);

    if(dir == 1) {
        if(time + 1 <= 20) {
            time++;
            payload.setFiringTime(time);

            itoa((100 + (time * 10)), strInt, 10);
            memcpy(lcdText, strInt, strlen(strInt));
            lcd.print(lcdText);
        }
    } else if (dir == -1) {
        if(time - 1 >= 0) {
            time--;
            payload.setFiringTime(time);

            itoa((100 + (time * 10)), strInt, 10);
            memcpy(lcdText, strInt, strlen(strInt));
            lcd.print(lcdText);
        }
    }
    payload.buildTransmission(buf, 7);

    // for (int i = 0; i < sizeof(buf); i++)
    // {
    //     Serial.println(buf[i], BIN);
    // }

    raw_driver.send(buf, sizeof(buf));

    // Serial.print("Left motor speed: ");
    // Serial.println(payload.getControllerDriveLeft());
    // Serial.print("Right motor speed: ");
    // Serial.println(payload.getControllerDriveRight());
    // Serial.print("Status: ");
    // Serial.println(payload.getStatus());
    // Serial.print("Angle: ");
    // Serial.println(payload.getAngle());
    // Serial.print("Encoder value: ");
    // Serial.println(encoder.getPosition());
    // Serial.println();


}

void onPinActivated(int pinNr)
{
    if (pinNr == ENABLE_PIN)
    {
        payload.setStatus(1);
    }
    else if (pinNr == PRIME_PIN && payload.getStatus() == 1)
    {
        payload.setStatus(3);
    }
    else if (pinNr == FIRE_PIN && payload.getStatus() == 3)
    {
        payload.setStatus(4);
    }
}

void onPinDeactivated(int pinNr)
{
    if (pinNr == ENABLE_PIN)
    {
        payload.setStatus(0);
    }
    else if (pinNr == PRIME_PIN && payload.getStatus() > 1)
    {
        payload.setStatus(1);
    }
    else if (pinNr == FIRE_PIN && payload.getStatus() > 3)
    {
        payload.setStatus(3);
    }
}