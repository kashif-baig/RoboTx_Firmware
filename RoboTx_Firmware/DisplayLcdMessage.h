#ifndef DISPLAYLCD_MESSAGE_H
#define DISPLAYLCD_MESSAGE_H

#include <MessagingLib.h>
#include "DisplayLcdTask.h"
//#include "Debug.h"

#define DL_CMD_CLEAR    'C'
#define DL_CMD_PRINT    'P'
#define DL_CMD_WAKEUP   'W'
#define DL_CMD_SLEEP    'S'

const char *DisplayLcdMessageType = "D";

/**
 * Deserializes request to display a string on LCD display.
 */
class DisplayLcdMessage : public IDeserializableMessage
{
public:
    DisplayLcdMessage(DisplayLcdTask *displayTask)
    {
        _displayTask = displayTask;
        _cmd = 0;
        _row = 0;
        _col = 0;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return DisplayLcdMessageType;
    }

    /**
     * Deserializes properties (fields) in the order they were serialized by the client.
     */
    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
    {
        switch (propertyIndex)
        {
        case 0:
        {
            _cmd = *propertyValue;
            if (_cmd == DL_CMD_CLEAR)
            {
                _displayTask->clear();
            }
            else if (_cmd == DL_CMD_WAKEUP)
            {
                _displayTask->lightOn();
            }
            else if (_cmd == DL_CMD_SLEEP)
            {
                _displayTask->lightOff();
            }
            break;
        }
        case 1:
        {
            _col = atoi(propertyValue);
            break;
        }
        case 2:
        {
            _row = atoi(propertyValue);
            break;
        }
        case 3:
        {
            if (_cmd == DL_CMD_PRINT)
            {
                _displayTask->printAt(_col, _row, propertyValue);
            }
            break;
        }
        }
    }

    /**
     * Processes the deserialized request.
     */
    void onEndDeserialize(bool messageComplete)
    {
        //Debug.println("Reply");
        if (messageComplete)
        {
        }
    }

private:

    DisplayLcdTask *_displayTask;
    char _cmd;
    uint8_t _row;
    uint8_t _col;
};

#endif