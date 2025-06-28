#ifndef IR_MESSAGE_H
#define IR_MESSAGE_H

#include <MessagingLib.h>
#include "Settings.h"

const char *IrMessageType = "I";

/**
 * Serializes an IR receiver command (button press).
 */
class IrMessage : public SerializableMessage
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
        // Serializes IR command as 4 digit hex string. Leading zeros are stripped.
        char cmdHex[5];
        char *cmdHexPtr = cmdHex;

        cmdHex[0] = pgm_read_byte(&_HexStr[(_irCommand >> 12) & 15]);
        cmdHex[1] = pgm_read_byte(&_HexStr[(_irCommand >> 8) & 15]);
        cmdHex[2] = pgm_read_byte(&_HexStr[(_irCommand >> 4) & 15]);
        cmdHex[3] = pgm_read_byte(&_HexStr[_irCommand & 15]);
        cmdHex[4] = 0;

        if (cmdHex[0] == '0')
        {
            cmdHexPtr++;

            if (cmdHex[1] == '0')
            {
                cmdHexPtr++;
            }
        }
        serializeProperty(cmdHexPtr);
    }

private:
    uint16_t _irCommand;
};

#endif
