#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

#include <Wire.h>
#include <SerLCD.h>
#include <Arduino.h>

class DisplayController
{
public:
    void init();
    void clear();
    void stringSetRegion(int x, int y, const char *text);
    void intSetRegion(int x, int y, int num);
    void print();

private:
    SerLCD m_lcd;
    char m_lcdText[33];
    char m_strInt[8];
    bool m_hasChanged;
};
#endif
