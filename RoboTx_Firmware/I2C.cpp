#include "I2C.h"

I2C::I2C(TwoWire *wireInstance, uint8_t devAddr) : wire(wireInstance), _devAddress(devAddr)
{

}

// ----------------------------------------------------------------------------
bool I2C::writeByte(uint8_t data, uint8_t holdBus)
{
    bool retval = true;

    wire->beginTransmission(_devAddress);
    if (!wire->write(data))
    {
        retval = false;
    }
    return wire->endTransmission(!(holdBus && retval))==0 && retval;
}

// ----------------------------------------------------------------------------
bool I2C::writeBytes(uint8_t *data, uint8_t dataSize)
{
    bool retval = true;

    wire->beginTransmission(_devAddress);

    if (!wire->write(data, dataSize))
    {
        retval = false;
    }
    return wire->endTransmission() == 0 && retval;
}

// ----------------------------------------------------------------------------
bool I2C::readByte(uint8_t *data)
{
    wire->requestFrom((int)_devAddress, (int)1, (int)true);
    if (wire->available())
    {
        *data = wire->read();
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool I2C::readBytes(uint8_t *data, uint8_t dataSize)
{
    wire->requestFrom((int)_devAddress, (int)dataSize, (int)true);

    uint8_t i = 0;
    for (; wire->available() && i < dataSize; i++)
    {
        data[i] = wire->read();
    }

    return i == dataSize;
}

// ----------------------------------------------------------------------------

bool I2C::writeRegister(uint8_t reg, uint8_t data)
{
    uint8_t cmd[2] = {reg, data};

    return writeBytes(cmd, sizeof(cmd));
}

bool I2C::readRegister(uint8_t reg, uint8_t *data, uint8_t dataSize = 1)
{
    if (!writeByte(reg, true))
    {
        return false;
    }
    return readBytes(data, dataSize);
}

uint16_t I2C::readRegisterWord(uint8_t reg)
{
    uint8_t data_buf[2];
    readRegister(reg, data_buf, 2);
    return ((uint16_t)data_buf[0] | (uint16_t)data_buf[1] << 8);
}

// ----------------------------------------------------------------------------

bool I2C::readBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data)
{
    uint8_t b;
    if (readRegister(regAddr, &b)) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
        return true;
    }
    return false;
}

bool I2C::writeBit(uint8_t reg, uint8_t bitNum, uint8_t data)
{
    uint8_t b;
    if (!readRegister(reg, &b))
    {
        return false;
    }

    b = (uint8_t)((data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum)));

    return writeRegister(reg, b);
}

bool I2C::writeBits(uint8_t reg, uint8_t bitStart, uint8_t bitLength, uint8_t data)
  {
    uint8_t b;
    if (!readRegister(reg, &b))
    {
      return false;
    }
    
    uint8_t mask = (uint8_t)(((1 << bitLength) - 1) << (bitStart - bitLength + 1));
    data <<= (bitStart - bitLength + 1);    // shift data into correct position
    data &= mask;                           // zero all non-important bits in data
    b &= (uint8_t)~(mask);                  // zero all important bits in existing byte
    b |= data;                              // combine data with existing byte
    
    return writeRegister(reg, b);
  }