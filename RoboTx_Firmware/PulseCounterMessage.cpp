#include "PulseCounterMessage.h"

const char *PulseCounterMessageType = "P";


void PulseCounterMessage::onEndDeserialize(bool messageComplete)
{
    if (messageComplete)
    {
        if (_enable)
        {
            // Disable pin that will be used for digital reading.
            _timerIO->enableInputByPin(PULSE_COUNTER_PIN, false);      //(BUTTON_2, false);
            // Disable same pin for analog reading.
            _analogTask->enableInputByPin(PULSE_COUNTER_PIN, false);

            _pulseCounterTask->enable(PULSE_COUNTER_PIN, _timeout, _trigger);
        }
        else
        {          
            // Disable pulse counter.
            _pulseCounterTask->disable();
        }
    }
}