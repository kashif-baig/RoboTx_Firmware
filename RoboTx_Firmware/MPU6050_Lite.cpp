#include "MPU6050_Lite.h"

bool MPU6050::begin()
{
  _i2c->begin();

  uint8_t deviceId;
  _i2c->readBits(RA_WHO_AM_I, WHO_AM_I_BIT, WHO_AM_I_LENGTH, &deviceId);

  if (!((deviceId == 0x34) || (deviceId == 0xC) || (deviceId == 0x3A)))
  {
    return false;
  }
  
  // Disable sleep
  _i2c->writeBit(RA_PWR_MGMT_1, PWR1_SLEEP_BIT, 0);

  // set low pass filter to on.
  _i2c->writeBits(RA_CONFIG, CFG_DLPF_CFG_BIT, CFG_DLPF_CFG_LENGTH, DLPF_BW_188);

  // Sets sample rate to 1000/1+9 = 100Hz
  _i2c->writeRegister(RA_SMPLRT_DIV, 0x09);

  // Disable FSync, 256Hz DLPF
  _i2c->writeRegister(RA_CONFIG, 0x00);
  // Disable gyro self tests, scale of 500 degrees/s
  _i2c->writeRegister(RA_GYRO_CONFIG, 0x08);
  // Disable accel self tests, scale of +-2g, no DHPF
  _i2c->writeRegister(RA_ACCEL_CONFIG, 0x00);

  // Disable sensor output to FIFO buffer
  _i2c->writeRegister(RA_FIFO_EN, 0x00);

  // set clock source
  _i2c->writeBits(RA_PWR_MGMT_1, PWR1_CLKSEL_BIT, PWR1_CLKSEL_LENGTH, CLOCK_PLL_XGYRO);

  return true;
}

void MPU6050::setAccelScale(uint8_t accelScale = 0)
{
  if (accelScale > 3)
  {
    accelScale = 3;
  }
  // set Accel scale
  _i2c->writeBits(RA_ACCEL_CONFIG, ACONFIG_AFS_SEL_BIT, ACONFIG_AFS_SEL_LENGTH, accelScale);
}

void MPU6050::setGyroScale(uint8_t gyroScale = 0)
{
  if (gyroScale > 3)
  {
    gyroScale = 3;
  }
  // set Gyro scale
  _i2c->writeBits(RA_GYRO_CONFIG, GCONFIG_FS_SEL_BIT, GCONFIG_FS_SEL_LENGTH, gyroScale);
}

// ----------------------------------------------------------------------------

void MPU6050::enable()
{
  _i2c->writeBit(RA_PWR_MGMT_1, PWR1_SLEEP_BIT, 0);
}

void MPU6050::disable()
{
  _i2c->writeBit(RA_PWR_MGMT_1, PWR1_SLEEP_BIT, 1);
}

// ----------------------------------------------------------------------------
void MPU6050::getAccelRaw(uint16_t *x, uint16_t *y, uint16_t *z)
{
  uint8_t buffer[6];

  _i2c->readRegister(RA_ACCEL_XOUT_H, buffer, sizeof(buffer));
  *x = (((uint16_t)buffer[0]) << 8) | buffer[1];
  *y = (((uint16_t)buffer[2]) << 8) | buffer[3];
  *z = (((uint16_t)buffer[4]) << 8) | buffer[5];
}

// ----------------------------------------------------------------------------
void MPU6050::getGyroRaw(uint16_t *x, uint16_t *y, uint16_t *z)
{
  uint8_t buffer[6];

  _i2c->readRegister(RA_GYRO_XOUT_H, buffer, sizeof(buffer));
  *x = (((uint16_t)buffer[0]) << 8) | buffer[1];
  *y = (((uint16_t)buffer[2]) << 8) | buffer[3];
  *z = (((uint16_t)buffer[4]) << 8) | buffer[5];
}
