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