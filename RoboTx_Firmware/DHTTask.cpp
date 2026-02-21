#include "DHTTask.h"

#if defined(DHT20_SENSOR_AVAILABLE)

void DHTTask::doEvents()
{
    if (!enabled())
    {
        return;
    }

    uint16_t timeDiff = compareTime();

    switch (_dhtTaskState)
    {
        case DHTTaskState_Delay:
        {
            if (timeDiff >= DHT_TASK_INTERVAL_MS)
            {
                setTime(timeDiff - DHT_TASK_INTERVAL_MS);
                _dhtTaskState = DHTTaskState_ReadTemp;
            }
            break;
        }
        case DHTTaskState_ReadTemp:
        {
            if (timeDiff >= 5)
            {
                setTime(timeDiff - 5);
                _dhtMessage->setTemperature(dht20.getTemperatureTenths());
                _dhtTaskState = DHTTaskState_ReadHumidity;
            }
            break;
        }
        case DHTTaskState_ReadHumidity:
        {
            if (timeDiff >= 5)
            {
                setTime(timeDiff - 5);
                _dhtMessage->setHumidity(dht20.getHumidityTenths());
                _dhtTaskState = DHTTaskState_Send;
            }
            break;
        }
        case DHTTaskState_Send:
        {
            if (timeDiff >= 5)
            {
                setTime(timeDiff - 5);
                _dhtMessage->serialize(&MsgSerial);
                _dhtTaskState = DHTTaskState_Delay;
            }
            break;
        }
    }
}

#endif