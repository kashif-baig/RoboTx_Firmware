#ifndef I2CREQUEST_MESSAGE_H
#define I2CREQUEST_MESSAGE_H

#include "ColourTask.h"
#include "LightMeterTask.h"
#include "DHTTask.h"
#include "AccelGyroTask.h"
#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "Settings.h"
// #include "Debug.h"

extern const char *I2CRequestMessageType;

/**
 * Deserializes request for configuring colour sensor.
 * Serializes response with colour values.
 */
class I2CRequestMessage : public IDeserializableMessage
{
public:
    I2CRequestMessage(ColourTask *colourTask, LightMeterTask *lightMeterTask, DHTTask *dhtTask, AccelGyroTask *accelGyroTask)
    {
        _colourTask = colourTask;
        _lightMeterTask = lightMeterTask;
        _dhtTask = dhtTask;
        _accelGyroTask = accelGyroTask;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return I2CRequestMessageType;
    }

protected:
    char _device;
    bool _enableRequested;
    uint8_t _integrationTimeIdx;
    uint8_t _gain;

    void onBeginDeserialize()
    {
        _device = 0;
        _enableRequested = false;
        _integrationTimeIdx = 2;
        _gain = 1;
    }

    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue);

    /**
     * Deserializes properties (fields) in the order they were serialized by the client.
     */
    void onEndDeserialize(bool messageComplete);

private:
    ColourTask *_colourTask;
    LightMeterTask *_lightMeterTask;
    DHTTask *_dhtTask;
    AccelGyroTask *_accelGyroTask;
};

#endif