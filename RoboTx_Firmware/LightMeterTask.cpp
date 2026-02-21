#include "LightMeterTask.h"

#if defined(LIGHTMETER_SENSOR_AVAILABLE)

void LightMeterTask::doEvents()
{
    if (!enabled())
    {
        return;
    }

    uint16_t timeDiff = compareTime();

    if (timeDiff >= LIGHTMETER_TASK_INTERVAL_MS)
    {
        // Set task timer and minimise time drift.
        setTime(timeDiff - LIGHTMETER_TASK_INTERVAL_MS);

        //if (_lightMeter.measurementReady(true))
        //{
            int32_t lux = _lightMeter.readLightLevel();

            //Debug.println(lux);

            _lightMeterMessage->setLux(lux);
            _lightMeterMessage->serialize(&MsgSerial);
        //}
    }
}

#endif