#include "LightMeterMessage.h"

const char *LightMeterMessageType = "U";

void LightMeterMessage::onSerialize()
{
    if (_lux < 0)
    {
        serializeProperty(""); // Need to do this here to send first property delimiter.
        getStream()->print(_lux);
    }
    else
    {
        serializeToHex(_lux);
    }
}