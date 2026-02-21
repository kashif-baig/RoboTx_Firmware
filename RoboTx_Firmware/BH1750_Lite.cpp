#include "BH1750_Lite.h"
#include <Arduino.h>

BH1750_Lite::BH1750_Lite(I2C *i2c) : _i2c(i2c)
{
}

bool BH1750_Lite::begin()
{
  // Configure sensor in specified mode and set default MTreg
  return (configure(CONTINUOUS_HIGH_RES_MODE) && setMTreg(BH1750_DEFAULT_MTREG));
}

bool BH1750_Lite::configure(Mode mode)
{
  bool retVal = false;

  // Check measurement mode is valid
  switch (mode)
  {
  case BH1750_Lite::UNCONFIGURED:
  case BH1750_Lite::CONTINUOUS_HIGH_RES_MODE:
  {
    retVal = _i2c->writeByte((uint8_t)mode);

    // Wait a millisecond to wake up
    delay(1);
    break;
  }
  default:
    // Invalid measurement mode
    break;
  }
  if (retVal)
  {
    BH1750_MODE = mode;
    return true;
  }
  return false;
}

bool BH1750_Lite::setMTreg(uint8_t MTreg)
{
  if (MTreg < BH1750_MTREG_MIN || MTreg > BH1750_MTREG_MAX)
  {
    return false;
  }
  bool retVal = false;
  // Send MTreg and the current mode to the sensor
  //   High bit: 01000_MT[7,6,5]
  //    Low bit: 011_MT[4,3,2,1,0]
  retVal = _i2c->writeByte((0b01000 << 3) | (MTreg >> 5));
  retVal = retVal && _i2c->writeByte((0b011 << 5) | (MTreg & 0b11111));
  retVal = retVal && _i2c->writeByte(BH1750_MODE);

  if (retVal)
  {
    BH1750_MTreg = MTreg;
    return true;
  }
  return false;
}

/**
 * Read light level from sensor
 * The return value range differs if the MTreg value is changed. The global
 * maximum value is noted in the square brackets.
 * @return Light level in lux (0.0 ~ 54612,5 [117758,203])
 * 	   -1 : no valid return value
 * 	   -2 : sensor not configured
 */
int32_t BH1750_Lite::readLightLevel()
{

  if (BH1750_MODE == UNCONFIGURED)
  {
    return -2;
  }

  // Measurement result will be stored here
  int32_t level = -1;

  // Read two bytes from the sensor, which are low and high parts of the sensor
  // value
  uint8_t data_buf[2];

  if (_i2c->readBytes(data_buf, 2))
  {
    level = ((uint16_t)data_buf[0] << 8 | data_buf[1]);

    if (BH1750_MTreg != BH1750_DEFAULT_MTREG)
    {
      level = (level * BH1750_DEFAULT_MTREG) / BH1750_MTreg;
    }
    // Convert raw value to lux
    level = (level * 5) / 6;
  }

  return level;
}