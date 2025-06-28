#include "PulseCounterTask.h"

void PulseCounterTask::doEvents()
{
    if (!_pulseInEnabled)
    {
        return;
    }

    uint16_t timeDiff = compareTime();

    if (timeDiff >= PULSE_COUNTER_TASK_INTERVAL_MS)
    {
        // Set task timer and minimise time drift.
        setTime(timeDiff - PULSE_COUNTER_TASK_INTERVAL_MS);
        
        uint16_t pulsePeriod = getPulseInPeriod();

        if (!_messageSender->messageQueued(_pulseCounterMessage))
        {
            _pulseCounterMessage->setPulsePeriod(pulsePeriod);
            _messageSender->queueMessage(_pulseCounterMessage);
        }
    }
}