#ifndef DISPLAY7SEG_TASK_H
#define DISPLAY7SEG_TASK_H

#include "Task.h"

#define ON  1
#define OFF  0

/**
 * Writes to and refreshes the LED 7 segment display.
 * The initial state of the task is disabled. This allows
 * the Arduino pins used for the display to be used by other
 * devices as long as the task remains in a disabled state.
 */
class Display7SegTask : public Task
{
public:
    Display7SegTask(uint8_t latchPin, // RCLK
                uint8_t clkPin,   // SCLK
                uint8_t dataPin   // DIO
                );

    // Enables or disables the display.
    void setEnabled(bool enabled = ON);
    // Returns true if the display is enabled, false otherwise.
    bool isEnabled() const;

    // Writes to the LED digit display.
    void write(const char *textstring, uint8_t rightAlign =0);

    // Sets the display brightness. Parameter value 0 = max, 3 = min.
    void setDisplayBrightness(uint8_t level);

    // Displays digits in inverse. May help if display is common cathode type.
    void setDisplayInverse(bool isInverse);

    // Set segments Right to Left.
    void setSegmentRTL(bool setRTL);

    /**
     * Set task to run using interrupts. Necessary if main application is in blocking state
     * for longer than a few milliseconds.
     */
    void runAsBackgroundTask(bool isBackgroundTask)
    {
        _isBackgroundTask = isBackgroundTask;
    }

    // Returns true if the task is running using interrupts.
    bool isBackgroundTask() const
    {
        return _isBackgroundTask;
    }

    // Refreshes the LED 7 segment display.
    void doEvents();

private:
    uint8_t _displayIdx = 0;

    bool _displayMasterEnabled = 0;
    uint8_t _displayXor = 0;

    const char DISPLAY_OVERFLOW_ERROR = 'E';

    uint8_t _displayMemory[4];// = {255, 255, 255, 255};

    #define DISPLAY_TIMER_SCALER_RELOAD 4

    uint8_t _displayTimerScaler = DISPLAY_TIMER_SCALER_RELOAD;
    uint8_t _displayBrightness = 3;

    uint8_t _latchPin;
    uint8_t _clkPin;
    uint8_t _dataPin;

    void writeSegmentValue(uint8_t Segment, uint8_t Value);

    bool _isBackgroundTask = false;

    // AVR port specfics
#if defined(AVR)
    volatile uint8_t *_latchPinPortReg;
    uint8_t _latchPinPortbit;
    volatile uint8_t *_clkPinPortReg;
    uint8_t _clkPinPortbit;
    volatile uint8_t *_dataPinPortReg;
    uint8_t _dataPinPortbit;
#endif

};

#endif
