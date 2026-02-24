#include "SerializableMessageLite.h"

void SerializableMessageLite::serialize(Print *outStream)
{
    this->stream = outStream;

    stream->write(MSG_START);
    stream->write(getMessageType());
    onSerialize();
    stream->write(MSG_END);
}


void SerializableMessageLite::serializeProperty(const char *propertyValue)
{
    stream->write(MSG_PROPERTY);
    stream->print(propertyValue);
}


Print *SerializableMessageLite::getStream() const
{
    return stream;
}

PROGMEM const char _HexStr[] = "0123456789ABCDEF";

const char * SerializableMessageLite::convertToHex(uint16_t value)
{
    static char valueHexOut[5];

    uint8_t i = 0;
    if (value >=1024)
    {
        valueHexOut[i++] = pgm_read_byte(&_HexStr[(value >> 12) & 15]);
    }
    if (value >= 256)
    {
        valueHexOut[i++] = pgm_read_byte(&_HexStr[(value >> 8) & 15]);
    }
    valueHexOut[i++] = pgm_read_byte(&_HexStr[(value >> 4) & 15]);
    valueHexOut[i++] = pgm_read_byte(&_HexStr[value & 15]);
    valueHexOut[i] = 0;

    return valueHexOut;
}