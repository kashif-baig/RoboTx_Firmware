#ifndef PULSE_COUNTER_MESSAGE_H
#define PULSE_COUNTER_MESSAGE_H

#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "TimerIO.h"
#include "PulseCounterTask.h"
#include "AnalogTask.h"

extern const char *PulseCounterMessageType;

class PulseCounterTask;

/**
 * Dserializes request to enable or disable pulse counter.
 */
class PulseCounterMessage : public SerializableMessageLite, public IDeserializableMessage
{
public:

    PulseCounterMessage(TimerIO *timerIO, PulseCounterTask *pulseCounterTask, AnalogTask *analogTask)
    {
        _timerIO = timerIO;
        _pulseCounterTask = pulseCounterTask;
        _analogTask = analogTask;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return PulseCounterMessageType;
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
                // First property is either 'F' or an integer indicating timeout.
                if (*propertyValue == 'F')
                {
                    _enable = false;
                }
                else
                {
                    _timeout = atoi(propertyValue);
                    _enable = _timeout > 0;
                }
                break;
            }
            case 1:
            {
                // Second property (if available) is the digital value to trigger the pulse on (1 ot 0).
                _trigger = *propertyValue == '1' ? 1 : 0;
                break;
            }
        }
    }

    /**
     * Processes the deserialized request.
     */
    void onEndDeserialize(bool messageComplete);
    
    // Sets the pulse period to serialize to the client.
    void setPulsePeriod(uint16_t pulsePeriod)
    {
        _pulsePeriod = pulsePeriod;
    }

    // Serializes pulse counter reading back to the client.
    void onSerialize()
    {
        serializeProperty("");
        getStream()->print(_pulsePeriod);
    }

private:

    TimerIO *_timerIO;
    PulseCounterTask *_pulseCounterTask;
    AnalogTask *_analogTask;

    uint16_t _pulsePeriod;

    bool _enable = false;
    // The number of milliseconds to wait for a pulse, before resetting pulse in period to 0.
    int16_t _timeout = 0;
    // Trigger pulse on LOW (0) or HIGH (1) input.
    uint8_t _trigger = 0;
};

#endif
