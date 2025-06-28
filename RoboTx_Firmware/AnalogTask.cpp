#include "AnalogTask.h"

#if defined(PIN_A6) && defined(PIN_A7) && !defined(__AVR_ATmega32U4__)
PROGMEM const uint8_t _analog_pin[ANALOG_PIN_COUNT] = {A0, A1, A2, A3, A4, A5, A6, A7};
#else
PROGMEM const uint8_t _analog_pin[ANALOG_PIN_COUNT] = {A0, A1, A2, A3, A4, A5};
#endif

void AnalogTask::initialize()
{
    for (uint8_t i = 0; i < ANALOG_PIN_COUNT; i++)
    {
        pinMode(pgm_read_byte(&_analog_pin[i]), INPUT);
    }
}

void AnalogTask::enableInputByPin(int8_t pin, bool on)
{
    for (uint8_t idx = 0; idx < ANALOG_PIN_COUNT; idx++)
    {
        if (pin == pgm_read_byte(&_analog_pin[idx]))
        {
            enableInputByIdx(idx, on);
            break;
        }
    }
}

int8_t AnalogTask::getInputPinByIdx(uint8_t pinIdx) const
{
    if (pinIdx < ANALOG_PIN_COUNT)
    {
        return pgm_read_byte(&_analog_pin[pinIdx]);
    }
    return -1;
}

void AnalogTask::doEvents()
{
    uint16_t timeDiff = compareTime();

    switch (_taskState)
    {
    case AnalogTaskState_Delay:
    {
        if (timeDiff >= _taskIntervalMs)
        {
            // Set task timer and minimise time drift.
            setTime(timeDiff - _taskIntervalMs);
            _taskState = AnalogTaskState_ReadValue;

            _readPinIdx = 0;
        }
        break;
    }
    case AnalogTaskState_ReadValue:
    {
        if (timeDiff >= ANALOG_TASK_READING_INTERVAL_MS)
        {
            setTime(timeDiff - ANALOG_TASK_READING_INTERVAL_MS);

            if (inputEnabledByIdx(_readPinIdx))
            {
                _analog_val[_readPinIdx] = analogRead(pgm_read_byte(&_analog_pin[_readPinIdx]));
            }
            _readPinIdx++;
            if (_readPinIdx >= ANALOG_PIN_COUNT)
            {
                _taskState = AnalogTaskState_Send;
            }
        }
        break;
    }
    case AnalogTaskState_Send:
    {
        _taskState = AnalogTaskState_Delay;

        if (enabled() && anyInputsEnabled())
        {
            _messageSender->queueMessageIfNotQueued(_analogMessage);
        }
        break;
    }
    }
}