#include "Settings.h"

#if defined(AVR)

void digitalWriteFast_(uint8_t pin, uint8_t val)
{
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *reg = portOutputRegister(port);

    writePortAVR(reg, bit, val);
}

#endif

void writeToStream(Print *stream, const char *str_P)
{
    if (!str_P)
        return;

    uint8_t i;
    char chr = pgm_read_byte(str_P++);

    do
    {
        uint8_t buf[16];

        for (i = 0; i < sizeof(buf) && chr != (char)0; i++)
        {
            buf[i] = chr;
            chr = pgm_read_byte(str_P++);
        }

        if (i > 0)
        {
            stream->write(buf, i);
        }
    } while (i > 0);
}

void convertToHex(char *valueHexOut, uint16_t value)
{
    // char valueHex[5];
    uint8_t i = 0;
    if (value >=1024)
    {
        valueHexOut[i++] = pgm_read_byte(&_HexStr[(value >> 12) & 15]);
    }
    valueHexOut[i++] = pgm_read_byte(&_HexStr[(value >> 8) & 15]);
    valueHexOut[i++] = pgm_read_byte(&_HexStr[(value >> 4) & 15]);
    valueHexOut[i++] = pgm_read_byte(&_HexStr[value & 15]);
    valueHexOut[i] = 0;
}