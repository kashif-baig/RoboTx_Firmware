#ifndef MPU6050_h_
#define MPU6050_h_

#include <stdint.h>
#include "I2C.h"


class MPU6050 {
public:

  MPU6050(I2C *i2c)
    : _i2c(i2c) {
  }

  // Initializes the MPU6050 sensor.
  bool begin();

  void getAccelRaw(uint16_t *x, uint16_t *y, uint16_t *z);  // get raw acceleration values
  void getGyroRaw(uint16_t *x, uint16_t *y, uint16_t *z);   // get gyro acceleration values

  void setAccelScale(uint8_t accelScale = 0);
  void setGyroScale(uint8_t gyroScale = 0);

  void enable();
  void disable();

private:
  I2C *const _i2c;
};

#define ADDRESS_AD0_LOW 0x68
// address pin low (GND), default for InvenSense evaluation board

#define ADDRESS_AD0_HIGH 0x69
#define MPU_DEFAULT_ADDRESS ADDRESS_AD0_LOW

#define RA_SMPLRT_DIV 0x19
#define RA_CONFIG 0x1A
#define RA_GYRO_CONFIG 0x1B
#define RA_ACCEL_CONFIG 0x1C
#define RA_FF_THR 0x1D
#define RA_FF_DUR 0x1E
#define RA_MOT_THR 0x1F
#define RA_MOT_DUR 0x20
#define RA_ZRMOT_THR 0x21
#define RA_ZRMOT_DUR 0x22
#define RA_FIFO_EN 0x23
#define RA_I2C_MST_CTRL 0x24

#define RA_I2C_SLV4_DI 0x35
#define RA_I2C_MST_STATUS 0x36
#define RA_INT_PIN_CFG 0x37
#define RA_INT_ENABLE 0x38
#define RA_DMP_INT_STATUS 0x39
#define RA_INT_STATUS 0x3A
#define RA_ACCEL_XOUT_H 0x3B
#define RA_ACCEL_XOUT_L 0x3C
#define RA_ACCEL_YOUT_H 0x3D
#define RA_ACCEL_YOUT_L 0x3E
#define RA_ACCEL_ZOUT_H 0x3F
#define RA_ACCEL_ZOUT_L 0x40
#define RA_TEMP_OUT_H 0x41
#define RA_TEMP_OUT_L 0x42
#define RA_GYRO_XOUT_H 0x43
#define RA_GYRO_XOUT_L 0x44
#define RA_GYRO_YOUT_H 0x45
#define RA_GYRO_YOUT_L 0x46
#define RA_GYRO_ZOUT_H 0x47
#define RA_GYRO_ZOUT_L 0x48

#define RA_MOT_DETECT_STATUS 0x61
#define RA_I2C_SLV0_DO 0x63
#define RA_I2C_SLV1_DO 0x64
#define RA_I2C_SLV2_DO 0x65
#define RA_I2C_SLV3_DO 0x66
#define RA_I2C_MST_DELAY_CTRL 0x67
#define RA_SIGNAL_PATH_RESET 0x68
#define RA_MOT_DETECT_CTRL 0x69
#define RA_USER_CTRL 0x6A
#define RA_PWR_MGMT_1 0x6B
#define RA_PWR_MGMT_2 0x6C
#define RA_BANK_SEL 0x6D
#define RA_MEM_START_ADDR 0x6E
#define RA_MEM_R_W 0x6F
#define RA_DMP_CFG_1 0x70
#define RA_DMP_CFG_2 0x71
#define RA_FIFO_COUNTH 0x72
#define RA_FIFO_COUNTL 0x73
#define RA_FIFO_R_W 0x74
#define RA_WHO_AM_I 0x75

#define CFG_EXT_SYNC_SET_BIT 5
#define CFG_EXT_SYNC_SET_LENGTH 3
#define CFG_DLPF_CFG_BIT 2
#define CFG_DLPF_CFG_LENGTH 3

#define DLPF_BW_256 0x00
#define DLPF_BW_188 0x01
#define DLPF_BW_98 0x02
#define DLPF_BW_42 0x03
#define DLPF_BW_20 0x04
#define DLPF_BW_10 0x05
#define DLPF_BW_5 0x06

#define GCONFIG_FS_SEL_BIT 4
#define GCONFIG_FS_SEL_LENGTH 2

#define GYRO_FS_250 0x00
#define GYRO_FS_500 0x01
#define GYRO_FS_1000 0x02
#define GYRO_FS_2000 0x03

#define ACONFIG_XA_ST_BIT 7
#define ACONFIG_YA_ST_BIT 6
#define ACONFIG_ZA_ST_BIT 5
#define ACONFIG_AFS_SEL_BIT 4
#define ACONFIG_AFS_SEL_LENGTH 2
#define ACONFIG_ACCEL_HPF_BIT 2
#define ACONFIG_ACCEL_HPF_LENGTH 3

#define ACCEL_FS_2 0x00
#define ACCEL_FS_4 0x01
#define ACCEL_FS_8 0x02
#define ACCEL_FS_16 0x03

#define PWR1_DEVICE_RESET_BIT 7
#define PWR1_SLEEP_BIT 6
#define PWR1_CYCLE_BIT 5
#define PWR1_TEMP_DIS_BIT 3
#define PWR1_CLKSEL_BIT 2
#define PWR1_CLKSEL_LENGTH 3

#define CLOCK_INTERNAL 0x00
#define CLOCK_PLL_XGYRO 0x01
#define CLOCK_PLL_YGYRO 0x02
#define CLOCK_PLL_ZGYRO 0x03
#define CLOCK_PLL_EXT32K 0x04
#define CLOCK_PLL_EXT19M 0x05
#define CLOCK_KEEP_RESET 0x07

#define RA_WHO_AM_I 0x75
#define WHO_AM_I_BIT 6
#define WHO_AM_I_LENGTH 6

#endif
