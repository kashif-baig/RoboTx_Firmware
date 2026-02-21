
#ifndef BH1750_LITE_H
#define BH1750_LITE_H

#include "I2C.h"

// No active state
#define BH1750_POWER_DOWN 0x00

// Waiting for measurement command
#define BH1750_POWER_ON 0x01

// Reset data register value - not accepted in POWER_DOWN mode
#define BH1750_RESET 0x07

// Default MTreg value
#define BH1750_DEFAULT_MTREG 69
#define BH1750_MTREG_MIN 31
#define BH1750_MTREG_MAX 254

class BH1750_Lite {
public:

  enum Mode : uint8_t {
    // same as Power Down
    UNCONFIGURED = 0,
    // Measurement at 1 lux resolution. Measurement time is approx 120ms.
    CONTINUOUS_HIGH_RES_MODE = 0x10,
  };

  BH1750_Lite(I2C *i2c);

  bool begin();

  bool configure(Mode mode);
  bool setMTreg(uint8_t MTreg);
  int32_t readLightLevel();
    
private:

  uint8_t BH1750_MTreg = (uint8_t)BH1750_DEFAULT_MTREG;
  Mode BH1750_MODE = UNCONFIGURED;
  I2C * const _i2c;
};

#endif