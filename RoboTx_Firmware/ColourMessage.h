#ifndef COLOUR_MESSAGE_H
#define COLOUR_MESSAGE_H

#include "ColourTask.h"
#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "Settings.h"

class ColourTask;

extern const char *ColourMessageType;

/**
 * Deserializes request for configuring colour sensor.
 * Serializes response with colour values.
 */
class ColourMessage : public SerializableMessageLite, public IDeserializableMessage
{
public:

    ColourMessage(ColourTask *colourTask)
    {
        _colourTask = colourTask;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return ColourMessageType;
    }

protected:
    bool _enableRequested;
    uint8_t _integrationTimeIdx;
    uint8_t _gain;

    void onBeginDeserialize()
    {
        _enableRequested = false;
        _integrationTimeIdx = 2;
        _gain = 1;
    }

    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
    {
        switch (propertyIndex)
        {
            case 0:
                _enableRequested = *propertyValue == 'T';
                break;
            case 1:
                _integrationTimeIdx = atoi(propertyValue);
                break;
            case 2:
                _gain = atoi(propertyValue);
                break;
        }
    }

    /**
     * Deserializes properties (fields) in the order they were serialized by the client.
     */
    void onEndDeserialize(bool messageComplete);

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
        valueHex[0] = pgm_read_byte(&_HexStr[(value >> 12) & 15]);
        valueHex[1] = pgm_read_byte(&_HexStr[(value >> 8) & 15]);
        valueHex[2] = pgm_read_byte(&_HexStr[(value >> 4) & 15]);
        valueHex[3] = pgm_read_byte(&_HexStr[value & 15]);
        valueHex[4] = 0;

        serializeProperty(valueHex);
    }
private:
    ColourTask *_colourTask;
};

#endif