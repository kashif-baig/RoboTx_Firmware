#ifndef TIMER_IO_H
#define TIMER_IO_H

#include <stdint.h>
#include <Arduino.h>
#include "Settings.h"

#define ON 1
#define OFF 0

#if !defined(DIGITAL_INPUT_PINS)
    #define DIGITAL_INPUT_PINS A0,A1,A2,A3,A4
#endif

// button state indicators
#define BUTTON_PRESSED_IND (0 << 6)
#define BUTTON_SHORT_RELEASE_IND (1 << 6)
#define BUTTON_LONG_PRESSED_IND (2 << 6)
#define BUTTON_LONG_RELEASE_IND (3 << 6)

#define MAX_INPUTS 5

/**
 * Uses timer based interrupt for digital input events (e.g. button press/release) and beeper (digital trigger).
 * The Arduino pin used for the beeper must not be used by any other device.
 */
class TimerIO
{
public:
    // Initializes this instance.
    void initialize(bool beeperInverted, bool inputsInverted);

    // For internal use only.
    void isrCallBack();

    // Engage the beeper, which is managed in the background. Period timing is in 100th of second
    void beep(uint16_t onPeriod = 20, uint16_t offPeriod = 0, uint8_t cycles = 1, uint16_t loopCycles = 1 /* 0=indefinitely */, uint16_t loopDelayPeriod = 0);

    // Use this to set the off period whilst the beeper is engaged,
    void setBeepOffPeriod(uint16_t offPeriod);

    // If the beeper sounds with a high signal, set to false. If the beeper sounds
    // with a low signal, set to true.
    void setBeeperInverted(bool inverted);

    // Queues a button action to the button queue, e.g BUTTON_1_PRESSED
    void queueButton(uint8_t button);

    // Pulls a button action from the button queue.
    uint8_t getButton();

    // Sets enabled state of an input ( 0 >= pinIdx < 5 ). When enabled, button presses are queued in the buffer.
    // In a disabled state, button is not queued.
    void enableInputByIdx(uint8_t pinIdx, bool enabled);

    // Sets enabled state of an input by its Arduino pin designation.
    // No action is taken if the specified pin is not registered for input.
    void enableInputByPin(int8_t pin, bool enabled);

    // Returns the Arduino pin by pin index. Returns -1 for an invalid index.
    int8_t getInputPinByIdx(uint8_t pinIdx) const;

    // Sets enabled state of all buttons.
    void setInputsEnabled(bool enabled);

    // Allows button inputs ( 0 >= pinIdx < 5 ) to be detected on LOW signal.
    void invertInputByIdx(uint8_t pinIdx, bool inverted);

    // To detect button on high signal, set to false. To detect button
    // on low signal, set to true;
    void setInputsInverted(bool inverted);

    // Sets the callback for user interrupt handler.
    void setUserInterrupt(void (*userInterruptHandler)() = 0);

private:
    void init();

    volatile uint8_t buttonBuffer[MAX_INPUTS * 2];
    volatile uint8_t buttonBufferCount = 0;
    volatile uint8_t button_write_pos = 0;
    volatile uint8_t button_read_pos = 0;

    volatile uint8_t buttonInverted[MAX_INPUTS];

    uint16_t buttonSampleIntervalCounter = 0;
    uint8_t buttonState[MAX_INPUTS] = {0, 0, 0, 0, 0}; // current up or down state
    uint8_t buttonPressTime[MAX_INPUTS] = {0, 0, 0, 0, 0};

    volatile bool beeperModifyInProgress = false;
    volatile uint8_t beeperState = 0; // 0=on period; 1=off period
    volatile uint16_t beeperOnPeriodReloadValue = 0;
    volatile uint16_t beeperOffPeriodReloadValue = 0;
    volatile uint16_t beeperPeriodCounter = 0;
    volatile uint8_t beeperCycleReloadValue = 0;
    volatile uint8_t beeperCycleCounter = 0;
    volatile uint16_t beeperLoopCycleCounter = 0;
    volatile uint16_t beeperLoopDelayPeriodReloadValue = 0;
    volatile uint8_t beeperInverted = 1;

    uint8_t beeperSignalOsc = 0;
    bool beeperEngaged = false;

    // AVR port specfics
#if defined(AVR)
    uint8_t volatile *BUTTON_PORT_IN_REG[MAX_INPUTS];
    volatile uint8_t BUTTON_BIT[MAX_INPUTS];

    volatile uint8_t *beeperPortReg;
    uint8_t beeperPortbit;
#endif

    volatile bool BUTTON_ENABLED[MAX_INPUTS] = {false, false, false, false, false};

    void writeBeeper(uint8_t value, bool force = false);
    void writeBeeperOsc();

    // Pointer to user interrupt with frequency of 1khz.
    void (*volatile userInterrupt)() = 0;
};

extern TimerIO TmrIO;

#endif
