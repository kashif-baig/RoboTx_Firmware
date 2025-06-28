#ifndef TASK_H
#define TASK_H

#include <Arduino.h>

// Abstract class for implementing a task.
class Task
{
public:
    // Process events or cycle to next FSM state. Normally invoked during the main application loop.
    //virtual void doEvents() = 0;

protected:
    // Records the current value of the milli second counter, minus
    // an optional offset.
    void setTime(uint16_t offsetMs = 0)
    {
        _timeMs = millis() - (int32_t)offsetMs;
    }

    // Returns the difference between the current value of the milli second counter
    // and the recorded value.
    uint16_t compareTime()
    {
        return millis() - _timeMs;
    }
private:

    uint32_t _timeMs;

};
#endif
