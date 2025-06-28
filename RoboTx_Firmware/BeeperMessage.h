#ifndef BEEPER_MESSAGE_H
#define BEEPER_MESSAGE_H

#include <MessagingLib.h>
#include "TimerIO.h"

#define BR_CMD_BEEP             'B'
#define BR_CMD_SET_OFF_PERIOD   'F'

const char *BeeperMessageType = "Z";

/**
 * NOTE the 'Trigger' in the API is synonymous with the 'Beeper'. 
 * Deserializes request to enage the beeper with a pattern.
 */
class BeeperMessage : public IDeserializableMessage
{
public:

    BeeperMessage(TimerIO *timerIO)
    {
        _timerIO = timerIO;
        _cmd = 0;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return BeeperMessageType;
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
            _onPeriod = atoi(propertyValue);
            break;
        case 2:
            _offPeriod = atoi(propertyValue);
            break;
        case 3:
            _cycles = atoi(propertyValue);
            break;
        case 4:
            _loopCycles = atoi(propertyValue);
            break;
        case 5:
            _loopDelayPeriod = atoi(propertyValue);
            break;
        }
    }

    /**
     * Processes the deserialized request.
     */
    void onEndDeserialize(bool messageComplete)
    {
        //debugln(F("-- end deserialize"));
        if (messageComplete)
        {
            if (_cmd == BR_CMD_BEEP)
            {
                _timerIO->beep(_onPeriod, _offPeriod, _cycles, _loopCycles, _loopDelayPeriod);
            }
            else if (_cmd == BR_CMD_SET_OFF_PERIOD)
            {
                _timerIO->setBeepOffPeriod(_offPeriod);
            }
        }
    }

private:
    TimerIO *_timerIO;
    char _cmd;
    uint16_t _onPeriod = 20;
    uint16_t _offPeriod = 0;
    uint8_t _cycles = 1;
    uint16_t _loopCycles = 1;
    uint16_t _loopDelayPeriod =0;
};

#endif