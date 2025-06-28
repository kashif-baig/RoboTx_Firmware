#ifndef ANALOG_TASK_H
#define ANALOG_TASK_H

#include "Task.h"
#include <MessagingLib.h>
#include "AnalogMessage.h"

/**
 * Default intervals determine how many times per second the analog pins (A0,A1,A2,A3,A4)
 * will be read and serialized. I.e. 1000 / (90 + 2 + 2 + 2 + 2 + 2) = 10 readings per second.
 */
#define ANALOG_TASK_DEFAULT_INTERVAL_MS 90
#define ANALOG_TASK_READING_INTERVAL_MS 2

#define ANALOG_MAX_SAMPLE_FREQ_HZ   50

#if defined(PIN_A6) && defined(PIN_A7) && !defined(__AVR_ATmega32U4__)
#define ANALOG_PIN_COUNT 8
#else
#define ANALOG_PIN_COUNT 6
#endif

/**
 * AnalogTask FSM states.
 */
enum AnalogTaskState : uint8_t
{
    AnalogTaskState_Delay,
    AnalogTaskState_ReadValue,
    AnalogTaskState_Send
};

class AnalogMessage;

/**
 * Reads and serializes analog pins (A0, A1, A2, A3, A4, A5, A6, A7) multiple times per second.
 */
class AnalogTask : public Task
{
private:
    AnalogTaskState _taskState;
    SerializableMessageSender *_messageSender;
    AnalogMessage *_analogMessage;

    uint16_t _taskIntervalMs;

    bool _enabled = true;

    uint16_t _analog_val[ANALOG_PIN_COUNT];// = {0,0,0,0,0,0};
    bool _analog_enabled[ANALOG_PIN_COUNT];// = {false,false,false,false,false,false};
    uint8_t _readPinIdx = 0;

public:

    /**
     * Constructs instance with reference to the message sender, which regulates the sending of
     * messages over Serial.
     */
    AnalogTask(SerializableMessageSender *messageSender)
    {
        _taskState = AnalogTaskState_Delay;

        setTime(ANALOG_TASK_DEFAULT_INTERVAL_MS);
        _taskIntervalMs = ANALOG_TASK_DEFAULT_INTERVAL_MS;
        _messageSender = messageSender;

        for (uint8_t i=0; i<ANALOG_PIN_COUNT; i++)
        {
            _analog_val[i] = 0;
            _analog_enabled[i] = false;
        }
    }

    /**
     * Initialize Analog pins for input mode.
     */
    void initialize();
    // {
    //     for (uint8_t i=0; i<ANALOG_PIN_COUNT; i++)
    //     {
    //         pinMode(_analog_pin[i], INPUT);
    //     }
    // }


    /**
     * Sets the number of times per second analog values are sampled and sent.
     */
    void setSampleRate(uint8_t sampleRateHz)
    {
        sampleRateHz = constrain(sampleRateHz, 1, ANALOG_MAX_SAMPLE_FREQ_HZ);
        _taskIntervalMs = (1000 / sampleRateHz) - (ANALOG_TASK_READING_INTERVAL_MS * ANALOG_PIN_COUNT);
    }

    /**
     * Establish relationship with the AnalogMessage that will serialize the analog readings.
     */
    void setAnalogMessage(AnalogMessage *analogMessage)
    {
        _analogMessage = analogMessage;
    }

    /**
     * Returns the maximum analog pin count.
     */
    uint8_t pinCount() const
    {
        return ANALOG_PIN_COUNT;
    }

    /**
     * Reads the enabled analog pins and serializes the values back to the client.
     */
    void doEvents();

    // Enables pin (specified by index where 0 >= pin <=5) for analog reading.
    void enableInputByIdx(uint8_t pinIdx, bool on)
    {
        //pin = constrain(pin, 0, ANALOG_PIN_COUNT-1);
        if (pinIdx >= ANALOG_PIN_COUNT)
        {
            return;
        }
        _analog_enabled[pinIdx] = on;
    }

    // Enables analog input by Arduino pin designation.
    void enableInputByPin(int8_t pin, bool on);

    // Returns the Arduino pin by pin index. Returns -1 for an invalid index.
    int8_t getInputPinByIdx(uint8_t pinIdx) const;

    /**
     * Returns true if pin specified by index is enabled. Returns false otherwise.
     */
    bool inputEnabledByIdx (uint8_t pinIdx) const
    {
        //pin = constrain(pin, 0, ANALOG_PIN_COUNT-1);
        if (pinIdx >= ANALOG_PIN_COUNT)
        {
            return false;
        }
        return _analog_enabled[pinIdx];
    }

    /**
     * Returns true if any input pin is enabled. Returns false otherwise.
     */
    bool anyInputsEnabled() const
    {
        for (uint8_t i=0; i<ANALOG_PIN_COUNT; i++)
        {
            if (_analog_enabled[i])
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns the analog value of a pin specified by index.
     */
    uint16_t inputValueByIdx(uint8_t pinIdx) const
    {
        //pin = constrain(pin, 0, ANALOG_PIN_COUNT-1);
        if (pinIdx >= ANALOG_PIN_COUNT)
        {
            return 0;
        }
        return _analog_val[pinIdx];
    }

    // Enables the task.
    void enable()
    {
        _enabled = true;
    }

    // Returns true if the task is enabled, false otherwise.
    bool enabled() const
    {
        return _enabled;
    }

    // Disables the task.
    void disable()
    {
        _enabled = false;
        // for (uint8_t i=0; i < ANALOG_PIN_COUNT; i++)
        // {
        //     enablePin(i, false);
        // }
    }


};

#endif