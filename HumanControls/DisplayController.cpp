#include "DisplayController.h"

void DisplayController::init()
{
  Wire.begin();
  this->m_lcd.begin(Wire);
  this->m_lcd.disableSystemMessages();
  this->m_lcd.setBacklight(255, 255, 255);
  this->m_lcd.setContrast(50);

  this->clear();
}

void DisplayController::clear()
{
  memset(m_lcdText, ' ', 32);
  this->m_lcd.clear();
  m_hasChanged = true;
}

void DisplayController::stringSetRegion(int x, int y, const char *text)
{
  memcpy(m_lcdText + ((y * 16) + x), text, strlen(text));
  m_hasChanged = true;
}

void DisplayController::intSetRegion(int x, int y, int num)
{
  itoa(num, m_strInt, 10);
  stringSetRegion(x, y, m_strInt);
  m_hasChanged = true;
}

void DisplayController::print()
{
  if (m_hasChanged)
  {
    m_lcd.print(m_lcdText);
  }
  m_hasChanged = false;
}