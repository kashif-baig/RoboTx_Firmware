#ifndef ACCEL_GYRO_MESSAGE_H
#define ACCEL_GYRO_MESSAGE_H

#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "Settings.h"

extern const char *AccelGyroMessageType;

/**
 * Serializes response with temperature and humidity values.
 */
class AccelGyroMessage : public SerializableMessageLite
{
public:

    AccelGyroMessage()
    {
        _valueType = 0;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return AccelGyroMessageType;
    }

    void serializeAccel(uint16_t x, uint16_t y, uint16_t z)
    {
        _valueType = 'A';
        _x=x;
        _y=y;
        _z=z;

        serialize(&MsgSerial);
    }

    void serializeGyro(uint16_t x, uint16_t y, uint16_t z)
    {
        _valueType = 'G';
        _x=x;
        _y=y;
        _z=z;

        serialize(&MsgSerial);
    }

protected:

    /**
     * Serializes the values read by the colour sensor.
     */
    void onSerialize();

    /**
     * Serializes a numeric value as a hex string.
     */
    void serializeToHex(uint16_t value)
    {
        char valueHex[5];
        convertToHex(valueHex, value);
        serializeProperty(valueHex);
    }

    private:
    char _valueType;
    uint16_t _x,_y,_z;
};

#endif