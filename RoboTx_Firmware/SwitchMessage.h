#ifndef SWITCH_MESSAGE_H
#define SWITCH_MESSAGE_H

#include <MessagingLib.h>
#include "SwitchManager.h"

const char *SwitchMessageType = "SW";

class SwitchMessage : public IDeserializableMessage
{
public:
    SwitchMessage(SwitchManager *switchManager)
    {
        _switchManager = switchManager;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return SwitchMessageType;
    }

    /**
     * Deserializes properties (fields) in the order they were serialized by the client.
     */
    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
    {
        switch (propertyIndex)
        {
        case 0:
            _switchNum = atoi(propertyValue);
            break;
        case 1:
            _switchState = *propertyValue == 'T';
            break;
        }
    }

    /**
     * Processes the deserialized request.
     */
    void onEndDeserialize(bool messageComplete)
    {
        if (messageComplete)
        {
            if (_switchNum != 0)
            {
                _switchManager->setSwitchOn(_switchNum, _switchState);
            }
        }
    }

private:
    SwitchManager *_switchManager;
    uint8_t _switchNum =0;
    bool _switchState = false;
};

#endif
