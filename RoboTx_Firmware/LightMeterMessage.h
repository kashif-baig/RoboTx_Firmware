#ifndef LIGHTMETER_MESSAGE_H
#define LIGHTMETER_MESSAGE_H

//#include "LightMeterTask.h"
#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "Settings.h"

//class LightMeterTask;

extern const char *LightMeterMessageType;

/**
 * Serializes response with LUX values.
 */
class LightMeterMessage : public SerializableMessageLite
{
public:

    LightMeterMessage()
    {

    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return LightMeterMessageType;
    }

    void setLux(int32_t lux)
    {
        _lux = lux;
    }

protected:

    /**
     * Serializes the values read by the colour sensor.
     */
    void onSerialize();

private:
    int32_t _lux;
};

#endif