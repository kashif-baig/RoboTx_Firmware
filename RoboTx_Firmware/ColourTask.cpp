#include "ColourTask.h"

#if defined(COLOUR_SENSING_ENABLED)

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

        _messageSender->queueMessageIfNotQueued(_colourMessage);

    }
}

#endif