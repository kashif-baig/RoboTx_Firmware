#ifndef BUTTON_MESSAGE_H
#define BUTTON_MESSAGE_H

#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "Settings.h"

const char *ButtonMessageType = "B";

/**
 * Serializes an input event, i.e. button press (or release) value.
 */
class ButtonMessage : public SerializableMessageLite
{
public:

    ButtonMessage()
    {

    }

    /**
     * Sets the value to serialize.
     */
    void setButtonValue(uint8_t buttionValue)
    {
        _buttonValue = buttionValue;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return ButtonMessageType;
    }

protected:
    void onSerialize()
    {
        serializeProperty(convertToHex(_buttonValue));
    }

private:

    uint8_t _buttonValue;
};

#endif