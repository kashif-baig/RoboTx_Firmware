#include "DHT20_Lite.h"

DHT20_Lite::DHT20_Lite(I2C *i2c) : _i2c(i2c)
{
}

bool DHT20_Lite::begin()
{
  uint8_t readCMD = 0x71;
  uint8_t data;

  _i2c->begin();

  // check if the IIC communication works
  _i2c->writeByte(readCMD);
  _i2c->readBytes(&data, 1);

  if ((data | 0x8) == 0)
  {
    return false;
  }
  if (data == 255)
    return false;
  return true;
}

int16_t DHT20_Lite::getTemperatureTenths()
{
  uint8_t readCMD[3] = {0xac, 0x33, 0x00};
  uint8_t data[6] = {0};

  int16_t temperature;

  _i2c->writeBytes(readCMD, 3);
  _i2c->readBytes(data, 6);

  uint32_t temp = data[3] & 0xff;
  uint32_t temp1 = data[4] & 0xff;
  int32_t rawData = 0;
  rawData = ((temp & 0xf) << 16) + (temp1 << 8) + (data[5]);

  temperature = ((rawData * 10) / 5242) - 500;

  return temperature;
}

uint16_t DHT20_Lite::getHumidityTenths()
{
  uint8_t readCMD[3] = {0xac, 0x33, 0x00};
  uint8_t data[6] = {0};

  uint16_t humidity;

  _i2c->writeBytes(readCMD, 3);
  _i2c->readBytes(data, 6);

  uint32_t temp = data[1] & 0xff;
  uint32_t temp1 = data[2] & 0xff;
  uint32_t rawData = 0;
  rawData = (temp << 12) + (temp1 << 4) + ((data[3] & 0xf0) >> 4);

  humidity = rawData / 1048;

  return humidity;
}
