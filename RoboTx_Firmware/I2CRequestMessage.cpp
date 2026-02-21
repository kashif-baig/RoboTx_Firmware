#include "I2CRequestMessage.h"

const char *I2CRequestMessageType = "I2C";

const char DEV_COLOUR = 'C';
const char DEV_ACCELEROMETER = 'X';
const char DEV_DHT = 'T';
const char DEV_LUM = 'L';

void I2CRequestMessage::onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
{
    switch (propertyIndex)
    {
    case 0:
        _device = *propertyValue;
        break;
    case 1:
        _enableRequested = *propertyValue == 'T';
        break;
    case 2:
    {
        if (_device == DEV_COLOUR)
            _integrationTimeIdx = atoi(propertyValue);
        break;
    }
    case 3:
    {
        if (_device == DEV_COLOUR)
            _gain = atoi(propertyValue);
        break;
    }
    }
}

void I2CRequestMessage::onEndDeserialize(bool messageComplete)
{
    if (messageComplete)
    {
        switch (_device)
        {
        case DEV_COLOUR:
        {
            if (_enableRequested)
            {
                _colourTask->enable(_integrationTimeIdx, _gain);
            }
            else
            {
                _colourTask->disable();
            }
            break;
        }
        case DEV_ACCELEROMETER:
        {
            if (_enableRequested)
            {
                _accelGyroTask->enable();
            }
            else
            {
                _accelGyroTask->disable();
            }
        }
        case DEV_DHT:
        {
            if (_enableRequested)
            {
                _dhtTask->enable();
            }
            else
            {
                _dhtTask->disable();
            }
            break;
        }
        case DEV_LUM:
        {
            if (_enableRequested)
            {
                // Debug.println("LM enabled");
                _lightMeterTask->enable();
            }
            else
            {
                _lightMeterTask->disable();
            }
            break;
        }
        default:
            break;
        }
    }
}
