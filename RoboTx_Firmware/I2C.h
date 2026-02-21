#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <Wire.h>

/// @brief This class provides a wrapper for the Wire library.
class I2C
{
public:
    /// @brief Instanciates I2C instance with reference to TwoWire instance and I2C device address.
    /// @param wireInstance Reference to TwoWire instance.
    /// @param devAddr I2C device address.
    I2C(TwoWire *wireInstance, uint8_t devAddr);

    /// @brief Initializes wire communication.
    void begin ()
    {
        wire->begin();
    }

    /// @brief Writes one byte to the device with option of holding the bus.
    /// @return true if successful, false otherwise.
    bool writeByte(uint8_t data, uint8_t holdBus = false);

    /// @brief Writes one or more bytes to the device.
    /// @return true if successful, false otherwise.
    bool writeBytes(uint8_t *data, uint8_t dataSize);

    /// @brief Reads a byte from the device.
    /// @return true if successful, false otherwise.
    bool readByte(uint8_t *data);

    /// @brief Reads one or more bytes from the device.
    /// @return true if successful, false otherwise.
    bool readBytes(uint8_t *data, uint8_t dataSize);

    /// @brief Writes a value to the specified device register.
    /// @return true if successful, false otherwise.
    bool writeRegister(uint8_t reg, uint8_t data);

    /// @brief Reads one or more values from the specified device register.
    /// @return true if successful, false otherwise.
    bool readRegister(uint8_t reg, uint8_t *data, uint8_t dataSize = 1);

    /// @brief Reads a 16 bit value from the specified device register.
    /// @return 16 bit value from device register.
    uint16_t readRegisterWord(uint8_t reg);

    /// @brief Read multiple bits from an 8-bit device register.
    /// @return true if successful, false otherwise.
    bool readBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);

    /// @brief Sets or clears specified bit of specified device register.
    /// @return true if successful, false otherwise.
    bool writeBit(uint8_t reg, uint8_t bitNum, uint8_t data);

    /// @brief Sets or clears specified group of bits of specified device register.
    /// @return true if successful, false otherwise.
    bool writeBits(uint8_t reg, uint8_t bitStart, uint8_t bitLength, uint8_t data);

private:

    TwoWire *const wire;
    const uint8_t _devAddress;
};

#endif
