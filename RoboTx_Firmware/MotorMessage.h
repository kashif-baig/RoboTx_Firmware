#ifndef MOTOR_MESSAGE_H
#define MOTOR_MESSAGE_H

#include <MessagingLib.h>
#include "Config.h"
#include "IMotorTask.h"

const char *MotorMessageType = "M";

#define MR_CMD_DRIVE 'D'
#define MR_CMD_SET_ACCEL 'A'
#define MR_CMD_OVERRIDE_ACCEL 'O'
#define MR_CMD_FOR_DURATION 'T'

/**
 * Deserializes request to drive a motor.
 */
class MotorMessage : public IDeserializableMessage
{
public:

    MotorMessage(Config *config)
    {
        _config = config;
        _motorNumber = 0;
        _cmdParam = 0;
        _cmdParamHasValue = false;
        _cmdParam2 = 0;
        _cmdParam2HasValue = false;
        _cmd = MR_CMD_DRIVE;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return MotorMessageType;
    }

    /**
     * Deserializes properties (fields) in the order they were serialized by the client.
     */
    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
    {
        switch (propertyIndex)
        {
        case 0:     // first property is motor number
        {
            _motorNumber = atoi(propertyValue);
            break;
        }
        case 1:     // second property is speed as percentage, or acceleration, or duration.
        {
            _cmdParamHasValue = strlen(propertyValue) > 0;
            _cmdParam = (int16_t)strtol(propertyValue, NULL, 16);
            break;
        }
        case 2:
        {
            _cmd = *propertyValue;
            break;
        }
        case 3:
        {
            _cmdParam2HasValue = strlen(propertyValue) > 0;
            _cmdParam2 = (int16_t)strtol(propertyValue, NULL, 16);
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
            // Drive the motor
            if (_config->motorTaskAvailable())
            {
                IMotorTask *motorTask = _config->getMotorTask();

                if (_cmd == MR_CMD_DRIVE
                    || (_cmd == MR_CMD_OVERRIDE_ACCEL && _cmdParamHasValue))
                {
                    motorTask->setTargetSpeed(_motorNumber, _cmdParam, _cmd == MR_CMD_OVERRIDE_ACCEL);
                    motorTask->setDuration(_motorNumber, MOTOR_TASK_INFINITE_DURATION);
                }
                else if (_cmd == MR_CMD_FOR_DURATION && _cmdParam2HasValue)
                {
                    //TmrIO.beep();
                    motorTask->setTargetSpeed(_motorNumber, _cmdParam, false);
                    motorTask->setDuration(_motorNumber, _cmdParam2HasValue ? _cmdParam2 : MOTOR_TASK_INFINITE_DURATION);
                }
                else if (_cmd == MR_CMD_OVERRIDE_ACCEL && !_cmdParamHasValue)
                {
                    motorTask->stopAccelerating(_motorNumber);
                }
                else if (_cmd == MR_CMD_SET_ACCEL && _cmdParamHasValue)
                {
                    motorTask->setAcceleration(_motorNumber, _cmdParam);
                }
            }
        }
    }

private:
    Config *_config;
    uint8_t _motorNumber;
    int16_t _cmdParam;
    int16_t _cmdParam2;
    bool _cmdParamHasValue;
    bool _cmdParam2HasValue;
    char _cmd;
};

#endif
