#ifndef ANALOG_MESSAGE_H
#define ANALOG_MESSAGE_H

#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "AnalogTask.h"
#include "Settings.h"

class AnalogTask;

extern const char *AnalogMessageType;

/**
 * Deserializes request for configuring analog pins A0, A1, A2, A3, A4 and A5. Serializes response
 * with values of those analog pins that have been enabled.
 */
class AnalogMessage : public SerializableMessageLite, public IDeserializableMessage
{
public:

    /**
     * Constructs instance using a reference to the AnalogTask responsible for reading the values
     * of the enabled analog pins.
     */
    AnalogMessage(AnalogTask *analogTask)
    {
        _analogTask = analogTask;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return AnalogMessageType;
    }

protected:

    bool _setSampleRate;

    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue);

    void onEndDeserialize(bool messageComplete)
    {
        if (messageComplete)
        {

        }
    }

    void onSerialize();

    /**
     * Serializes analog pin value to 3 digit hex string if enabled, blank string otherwise.
     */
    void serializeToHex(bool enabled, uint16_t value)
    {
        if (enabled)
        {
            char valueHex[5];
            // valueHex[0] = pgm_read_byte(&_HexStr[(value >> 8) & 15]);
            // valueHex[1] = pgm_read_byte(&_HexStr[(value >> 4) & 15]);
            // valueHex[2] = pgm_read_byte(&_HexStr[value & 15]);
            // valueHex[3] = 0;
            convertToHex(valueHex, value);
            serializeProperty(valueHex);
        }
        else
        {
            serializeProperty("");
        }
    }

private:

    AnalogTask *_analogTask;
};

#endif