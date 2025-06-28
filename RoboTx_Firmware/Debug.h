#ifndef DEBUG_H
#define DEBUG_H

#include <MessagingLib.h>
#include "Settings.h"

#if defined(DEBUG_ENABLED)

extern const char *DebugMessageType;// = "DB";

class DebugMessage : public SerializableMessage
{
    int32_t _val;
    const char *_msg;
    char _cmd = 0;

public:

    DebugMessage()
    {

    }

    void println(int32_t val)
    {
        _val = val;
        _cmd = 'n';

        serialize(&MsgSerial);
    }

    void println(const char *msg)
    {
        _msg = msg;
        _cmd = 's';

        serialize(&MsgSerial);
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return DebugMessageType;
    }

protected:
    void onSerialize()
    {
        if (_cmd == 'n')
        {
            char valStr[11];
            ltoa(_val, valStr, 10);
            serializeProperty(valStr);
        }
        else if (_cmd == 's')
        {
            serializeProperty(_msg);
        }
    }
};

extern DebugMessage Debug;// = DebugMessage();
#endif
#endif
