#ifndef SERVO_MESSAGE_H
#define SERVO_MESSAGE_H

#include <MessagingLib.h>
#include "ServoManager.h"
#include "Config.h"

// Avoid hex values for command.
#define SV_CMD_STOP     'S'
#define SV_CMD_RANGE    'R'
#define SV_CMD_SPEEDSTEP 'P'

const char *ServoMessageType = "V";

/**
 * Deserializes request for setting the angle of a servo motor.
 */
class ServoMessage : public IDeserializableMessage
{
public:

    /**
     * Constructs instance using references to the servo motor objects.
     */
    ServoMessage(ServoManager *servoManager/*, Config *config*/)
    {
        _servoManager = servoManager;
        _servoNumber = 0;
        //_config = config;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return ServoMessageType;
    }

    /**
     * Deserializes properties (fields) in the order they were serialized by the client.
     */
    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
    {
        // First property is the servo number, next property is either a command or the angle value.
        // Subsequent properties are parameters for specified command.
        switch (propertyIndex)
        {
        case 0:
        {
            _servoNumber = atoi(propertyValue);
            _cmd = 0;
            break;
        }
        case 1:
        {
            if (*propertyValue == SV_CMD_STOP || *propertyValue == SV_CMD_RANGE || *propertyValue == SV_CMD_SPEEDSTEP)
            {
                _cmd = *propertyValue;
            }
            else
            {
                _param1 = strtol(propertyValue, NULL, 16);
            }
            break;
        }
        case 2:
        {
            _param1 = strtol(propertyValue, NULL, 16);
            break;
        }
        case 3:
        {
            _param2 = strtol(propertyValue, NULL, 16);
            break;
        }
        }
    }

    /**
     * Processes the deserialized request.
     */
    void onEndDeserialize(bool messageComplete)
    {
        if (messageComplete)
        {
            if (_cmd == SV_CMD_STOP)
            {
                _servoManager->disableServo(_servoNumber);
            }
            else if (_cmd == SV_CMD_RANGE)
            {
                _servoManager->setServoRange(_servoNumber, _param1, _param2);
            }
            else if (_cmd == SV_CMD_SPEEDSTEP)
            {
                _servoManager->setServoSpeedStep(_servoNumber, _param1);
            }
            else
            {
                _servoManager->setServoPulse(_servoNumber, _param1);
            }
        }
    }

private:
    int16_t _param1, _param2;
    char _cmd;
    ServoManager *_servoManager;
    //Config *_config;
    uint8_t _servoNumber;
};

#endif