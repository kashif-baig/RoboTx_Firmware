#include "DHTMessage.h"

const char *DHTMessageType = "HT";

void DHTMessage::onSerialize()
{
    serializeProperty("");

    if (_temperatureCTenths >= -400 && _temperatureCTenths <=800)
    {
        getStream()->print(_temperatureCTenths);
    }
    else
    {
        getStream()->print('E');
    }
    serializeProperty("");
    if (_humidityPCTenths >= 0 && _humidityPCTenths <=1000)
    {
        getStream()->print(_humidityPCTenths);
    }
    else
    {
        getStream()->print('E');
    }

}