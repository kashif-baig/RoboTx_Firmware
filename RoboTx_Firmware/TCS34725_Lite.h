#ifndef TCS34725_Lite_H
#define TCS34725_Lite_H

#include "I2C.h"

#define TCS_ID (0x12) ///< 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727
#define TCS_COMMAND_BIT (0x80)
#define TCS_CDATAL (0x14) ///< Clear channel data
#define TCS_CDATAH (0x15)
#define TCS_RDATAL (0x16) ///< Red channel data
#define TCS_RDATAH (0x17)
#define TCS_GDATAL (0x18) ///< Green channel data
#define TCS_GDATAH (0x19)
#define TCS_BDATAL (0x1A) ///< Blue channel data
#define TCS_BDATAH (0x1B)

#define TCS_ENABLE (0x00)
#define TCS_ENABLE_AIEN (0x10) ///< RGBC Interrupt Enable
#define TCS_ENABLE_WEN (0x08)  ///< Wait enable - Writing 1 activates the wait timer
#define TCS_ENABLE_AEN (0x02)  ///< RGBC Enable - Writing 1 actives the ADC, 0 disables it
#define TCS_ENABLE_PON (0x01)  ///< Power on - Writing 1 activates the internal oscillator, 0 disables it
#define TCS_ATIME (0x01)       ///< Integration time

#define TCS_CONTROL (0x0F) ///< Set the gain level for the sensor

enum : uint8_t
{
  TCS_INTEGRATIONTIME_2_4MS = 0xFF, ///<  2.4ms - 1 cycle    - Max Count: 1024
  TCS_INTEGRATIONTIME_24MS = 0xF6,  ///<  24ms  - 10 cycles  - Max Count: 10240
  TCS_INTEGRATIONTIME_50MS = 0xEB,  ///<  50ms  - 20 cycles  - Max Count: 20480
  TCS_INTEGRATIONTIME_101MS = 0xD5, ///<  101ms - 42 cycles  - Max Count: 43008
  TCS_INTEGRATIONTIME_154MS = 0xC0, ///<  154ms - 64 cycles  - Max Count: 65535
  TCS_INTEGRATIONTIME_700MS = 0x00  ///<  700ms - 256 cycles - Max Count: 65535
};

enum : uint8_t
{
  TCS_GAIN_1X = 0x00,  ///<  No gain
  TCS_GAIN_4X = 0x01,  ///<  4x gain
  TCS_GAIN_16X = 0x02, ///<  16x gain
  TCS_GAIN_60X = 0x03  ///<  60x gain
};

/**
 * Configures and reads values from I2C colour sensing module.
 */
class TCS34725_Lite
{
public:
  TCS34725_Lite(I2C *i2c, uint8_t it = TCS_INTEGRATIONTIME_50MS, uint8_t gain = TCS_GAIN_4X)
      : _i2c(i2c)
  {
    _tcsIntegrationTime = it;
    _tcsGain = gain;
  }

  void getRGBC(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
  {
    *c = _i2c->readRegisterWord(TCS_COMMAND_BIT | TCS_CDATAL);
    *r = _i2c->readRegisterWord(TCS_COMMAND_BIT | TCS_RDATAL);
    *g = _i2c->readRegisterWord(TCS_COMMAND_BIT | TCS_GDATAL);
    *b = _i2c->readRegisterWord(TCS_COMMAND_BIT | TCS_BDATAL);
  }

  bool begin()
  {
    _i2c->begin();
    uint8_t x = 0;

    _i2c->readRegister(TCS_COMMAND_BIT | TCS_ID, &x);
    if ((x != 0x44) && (x != 0x10))
    {
      return false;
    }

    setIntegrationtime(_tcsIntegrationTime);
    setGain(_tcsGain);
    enable();
    return true;
  }

  void enable()
  {
    uint8_t data = TCS_ENABLE_PON;
    _i2c->writeRegister(TCS_COMMAND_BIT | TCS_ENABLE, data);
    data = TCS_ENABLE_PON | TCS_ENABLE_AEN;
    delay(3);
    _i2c->writeRegister(TCS_COMMAND_BIT | TCS_ENABLE, data);
  }

  void disable()
  {
    uint8_t reg = 0;
    _i2c->readRegister(TCS_COMMAND_BIT | TCS_ENABLE, &reg);
    reg = reg & ~(TCS_ENABLE_PON | TCS_ENABLE_AEN);
    _i2c->writeRegister(TCS_COMMAND_BIT | TCS_ENABLE, reg);
  }

  void setIntegrationtime(uint8_t it)
  {
    uint8_t data = it;
    _i2c->writeRegister(TCS_COMMAND_BIT | TCS_ATIME, data);
    _tcsIntegrationTime = it;
  }

  void setGain(uint8_t gain)
  {
    uint8_t data = gain;
    _i2c->writeRegister(TCS_COMMAND_BIT | TCS_CONTROL, data);
    _tcsGain = gain;
  }

private:
  I2C *const _i2c;
  uint8_t _tcsGain;
  uint8_t _tcsIntegrationTime;
};

#endif