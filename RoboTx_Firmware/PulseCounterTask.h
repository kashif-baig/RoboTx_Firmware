#ifndef PULSE_COUNTER_TASK_H
#define PULSE_COUNTER_TASK_H

#include <MessagingLib.h>
#include "PulseCounterMessage.h"
#include "Task.h"
#include <Arduino.h>

#define PULSE_COUNTER_TASK_INTERVAL_MS 50
#define PULSE_COUNTER_PIN   A2

class PulseCounterMessage;

/**
 * Calculates period of input pulses on a designated Arduino pin.
 * The initial state of the task is disabled. This allows the designated
 * Arduino pin to be used by other devices as long as the task remains
 * in a disabled state.
 */
class PulseCounterTask : public Task
{
public:
    PulseCounterTask(SerializableMessageSender *messageSender)
    {
        _messageSender = messageSender;
    }

    // Must set instance of PulseCounterMessage in order to serialize pulse period to client.
    void setPulseCounterMessage(PulseCounterMessage *pulseCounterMessage)
    {
        _pulseCounterMessage = pulseCounterMessage;
    }

    // Initializes and enables the pulse counter. Used for counting pulses applied to an input pin. Max pulse frequency 500hz.
    void enable(uint8_t pin = A2,        // input pin
              uint16_t timeOut = 3000, // the number of milliseconds to wait for a pulse, before resetting pulse in period to 0.
              uint8_t trigger = LOW    // trigger counter on either rising or falling edge
    )
    {
        timeOut = constrain(timeOut, PULSE_COUNTER_TASK_INTERVAL_MS + 50, 10000);

        _pulseInWriteInProgress = 1;

        pinMode(pin, INPUT);
        pulseInBit = digitalPinToBitMask(pin);
        uint8_t port = digitalPinToPort(pin);
        _portInpReg = portInputRegister(port);

        _pulseInTimeOut = timeOut;
        _pulseInPin = pin;
        _pulseInPeriodCounter = timeOut;
        _pulseInPeriod_volatile = 0;
        _pulseInPeriod_safe = 0;
        _pulseInState = 0;
        _pulseInTrigger = trigger;
        //pulseInTotalCount_volatile = 0;
        //pulseInTotalCount_safe = 0;

        _pulseInWriteInProgress = 0;
        _pulseInEnabled = true;

        setTime();
    }

    uint8_t getPulseInPin() const
    {
        return _pulseInPin;
    }

    // Disables the pulse counter.
    void disable()
    {
        _pulseInEnabled = false;
    }

    // Gets the period of the most recent pulse (in milliseconds).
    uint16_t getPulseInPeriod()
    {
        uint16_t period;

        _pulseInReadInProgress = 1;
        period = _pulseInPeriod_safe;
        _pulseInReadInProgress = 0;

        return period;
    }

    // Serializes the pulse period at the set interval of PULSE_COUNTER_TASK_INTERVAL_MS milliseconds.
    void doEvents();

    // Measures the input pulse interval. Max pulse frequency is 500hz, and should ideally be a square wave.
    void doISR()
    {
        if (_pulseInEnabled && !_pulseInWriteInProgress)
        {
            uint8_t pulseInStateNow = (*_portInpReg & pulseInBit ? 1 : 0);

            // Has the state of the pulse changed?
            if ((_pulseInState != pulseInStateNow) && (pulseInStateNow == _pulseInTrigger))
            {
                //pulseInTotalCount_volatile++;

                _pulseInPeriod_volatile = (_pulseInPeriodCounter >= _pulseInTimeOut ? 0 : _pulseInPeriodCounter);
                _pulseInPeriodCounter = 0;
            }
            else
            {
                if (_pulseInPeriodCounter < _pulseInTimeOut)
                {
                    _pulseInPeriodCounter++;
                }
                else
                {
                    _pulseInPeriod_volatile = 0;
                }
            }

            if (!_pulseInReadInProgress)
            {
                _pulseInPeriod_safe = _pulseInPeriod_volatile;
                //pulseInTotalCount_safe = pulseInTotalCount_volatile;
            }

            if (_pulseInState != pulseInStateNow)
            {
                _pulseInState = pulseInStateNow;
            }
        }
    }

private:
    SerializableMessageSender *_messageSender;
    PulseCounterMessage *_pulseCounterMessage;

    uint8_t pulseInBit;
    //uint8_t pulseInPort;

#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
   const volatile uint16_t *_portInpReg;
#else
   volatile uint8_t *_portInpReg;
#endif

    volatile uint8_t _pulseInEnabled = false;
    volatile uint8_t _pulseInReadInProgress = 0;
    volatile uint8_t _pulseInWriteInProgress = 0;

    volatile uint16_t _pulseInTimeOut;// = 3000; // time frame for measuring pulse period.
    volatile uint8_t _pulseInPin;// = A2;
    volatile uint16_t _pulseInPeriodCounter;// = 3000;
    volatile uint8_t _pulseInTrigger = LOW; // trigger on LOW or HIGH
    volatile uint16_t _pulseInPeriod_volatile = 0;
    volatile uint16_t _pulseInPeriod_safe = 0;
    volatile uint8_t _pulseInState = 0;
    volatile uint32_t _pulseInTotalCount_volatile = 0;
    volatile uint32_t _pulseInTotalCount_safe = 0;
};

#endif
