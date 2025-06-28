#include "AnalogMessage.h"

const char *AnalogMessageType = "A";

void AnalogMessage::onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
{
    /**
     * The first property is the request for setting the sample frequency. The second property is the sample rate.
     */
    switch (propertyIndex)
    {
    case 0:
        _setSampleRate = *propertyValue == 'S';
        break;
    case 1:
    {
        if (_setSampleRate)
        {
            _analogTask->setSampleRate(atoi(propertyValue));
        }
        break;
    }
    }
}

void AnalogMessage::onSerialize()
{
    for(uint8_t i=0; i<_analogTask->pinCount(); i++)
    {
        serializeToHex(_analogTask->inputEnabledByIdx(i), _analogTask->inputValueByIdx(i));
    }
}