#ifndef DISPLAY7SEG_MESSAGE_H
#define DISPLAY7SEG_MESSAGE_H

#include <MessagingLib.h>
#include "Display7SegTask.h"

#define D7_CMD_WRITE        'W'

const char *Display7SegMessageType = "D7";

/**
 * Deserializes request to display a string on (or blink) the 7 segment LED display.
 */
class Display7SegMessage : public IDeserializableMessage
{
public:

    Display7SegMessage(Display7SegTask *displayTask)
    {
        _displayTask = displayTask;
        _cmd = 0;
        _paramText[0] = 0;
        _paramValue = 0;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return Display7SegMessageType;
    }

    /**
     * Deserializes properties (fields) in the order they were serialized by the client.
     */
    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
    {
        switch (propertyIndex)
        {
        case 0:
            _cmd = *propertyValue;
            break;
        case 1:
            strncat(_paramText, propertyValue, sizeof(_paramText) - 1);
            break;
        case 2:
            _paramValue = atoi(propertyValue);
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
            if (!_displayTask->isEnabled())
            {
                _displayTask->setEnabled(true);
            }

            switch (_cmd)
            {
            case D7_CMD_WRITE:
            {
                _displayTask->write(_paramText, _paramValue);
                break;
            }
            }
        }
    }

private:
    Display7SegTask *_displayTask;
    char _cmd;
    char _paramText[9];
    uint8_t _paramValue;
};

#endif