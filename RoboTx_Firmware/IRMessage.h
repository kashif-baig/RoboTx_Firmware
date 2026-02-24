#ifndef IR_MESSAGE_H
#define IR_MESSAGE_H

#include "SerializableMessageLite.h"
#include "Settings.h"

const char *IrMessageType = "I";

/**
 * Serializes an IR receiver command (button press).
 */
class IrMessage : public SerializableMessageLite
{
public:

    IrMessage()
    {

    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return IrMessageType;
    }

    /**
     * Sets the IR command to serialize.
     */
    void setCommand (uint16_t irCommand)
    {
        _irCommand = irCommand;
    }

protected:
    void onSerialize()
    {
        // Serializes IR command as 4 digit hex string.
        serializeProperty(convertToHex(_irCommand));
    }

private:
    uint16_t _irCommand;
};

#endif
