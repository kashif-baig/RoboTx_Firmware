#ifndef DHT20_LITE_H
#define DHT20_LITE_H

#include "I2C.h"

/// @brief Lightweight implementation of DHT20 library.
class DHT20_Lite
{
public:
  /*!
   * @brief Construct the class.
   * @param I2C instance pointer.
   */
  DHT20_Lite(I2C *i2c);

  /**
   * @brief init function
   * @return Return true if initialization succeeds, otherwise return false.
   */
  bool begin();

  /**
   * @brief Get ambient temperature, unit in tenths of centigrade.
   * @return ambient temperature, measurement range: -40°C ~ 80°C
   */
  int16_t getTemperatureTenths();

  /**
   * @brief Get relative humidity, unit in tenths of percent: %RH.
   * @return relative humidity, measurement range: 0-100%
   */
  uint16_t getHumidityTenths();

private:
  I2C * const _i2c;
};

#endif