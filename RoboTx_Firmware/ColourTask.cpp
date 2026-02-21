#include "ColourTask.h"

#if defined(COLOUR_SENSOR_AVAILABLE)

void ColourTask::doEvents()
{
    if (!enabled())
    {
        return;
    }

    uint16_t timeDiff = compareTime();

    if (timeDiff >= _colourTaskIntervalMs)
    {
        // Set task timer and minimise time drift.
        setTime(timeDiff - _colourTaskIntervalMs);
        
        setRGB();

        _colourMessage->serialize(&MsgSerial);

    }
}

#endif