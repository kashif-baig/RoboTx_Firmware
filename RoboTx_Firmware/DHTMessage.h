#ifndef DHT_MESSAGE_H
#define DHT_MESSAGE_H

#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "Settings.h"

extern const char *DHTMessageType;

/**
 * Serializes response with temperature and humidity values.
 */
class DHTMessage : public SerializableMessageLite
{
public:

    DHTMessage()
    {

    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return DHTMessageType;
    }

    void setTemperature(int16_t tempTenths)
    {
        _temperatureCTenths = tempTenths;
    }

    void setHumidity(int16_t humidityTenths)
    {
        _humidityPCTenths = humidityTenths;
    }

protected:

    /**
     * Serializes the values read by the colour sensor.
     */
    void onSerialize();

private:
    int16_t _temperatureCTenths;
    int16_t _humidityPCTenths;
};

#endif