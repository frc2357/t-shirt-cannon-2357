#include "CommsI2CSlave.h"

CommsI2CSlave::CommsI2CSlave(int myAddress, int preambleLength, uint8_t *data, uint8_t datLen) : CommsI2CBase(myAddress, preambleLength)
{

  m_datLen = datLen;
  m_data = data;
}

void CommsI2CSlave::init(void (*recFunction)(int), void (*reqFunction)(void))
{
  CommsI2CBase::init();

  // Set up events
  Wire.onReceive(recFunction);
  Wire.onRequest(reqFunction);
}

// Function to switch the location m_data points to
void CommsI2CSlave::updateData(uint8_t *data, const uint8_t datLen)
{
  m_datLen = datLen;
  m_data = data;
}

void CommsI2CSlave::sendBytes()
{
  // Write preamble
  for (int i = 0; i < m_preambleLength; i++)
  {
    Wire.write(PREAMBLE_VALUE);
  }

  // Write data
  for (int i = 0; i < m_datLen; i++)
  {
    Wire.write(m_data[i]);
    Serial.println(m_data[i], BIN);
  }
  Serial.println();
}

const bool CommsI2CSlave::getBytes()
{
  return CommsI2CBase::getBytes(m_buffer, BUFFER_LEN, m_data, m_datLen);
}

size_t CommsI2CSlave::write(uint8_t ch)
{
  return 0;
}