#ifndef LIGHTMETER_MESSAGE_H
#define LIGHTMETER_MESSAGE_H

//#include "LightMeterTask.h"
#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "Settings.h"

//class LightMeterTask;

extern const char *LightMeterMessageType;

/**
 * Serializes response with LUX values.
 */
class LightMeterMessage : public SerializableMessageLite
{
public:

    LightMeterMessage()
    {

    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return LightMeterMessageType;
    }

    void setLux(int32_t lux)
    {
        _lux = lux;
    }

protected:

    /**
     * Serializes the values read by the colour sensor.
     */
    void onSerialize();

    /**
     * Serializes a numeric value as a hex string.
     */
    void serializeToHex(uint16_t value)
    {
        char valueHex[5];
        // valueHex[0] = pgm_read_byte(&_HexStr[(value >> 12) & 15]);
        // valueHex[1] = pgm_read_byte(&_HexStr[(value >> 8) & 15]);
        // valueHex[2] = pgm_read_byte(&_HexStr[(value >> 4) & 15]);
        // valueHex[3] = pgm_read_byte(&_HexStr[value & 15]);
        // valueHex[4] = 0;
        convertToHex(valueHex, value);
        serializeProperty(valueHex);
    }
private:
    int32_t _lux;
};

#endif