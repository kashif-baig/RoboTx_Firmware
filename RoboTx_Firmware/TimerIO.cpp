#include "TimerIO.h"

#define BUTTON_SAMPLE_FREQ_HZ 100
#define BUTTON_SAMPLE_INTERVAL (1000 / BUTTON_SAMPLE_FREQ_HZ)
#define BUTTON_REPEAT_THRESHOLD 48


TimerIO TmrIO;

#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
#include "FspTimer.h"
FspTimer r4Timer;
bool beginTimer(float rate);
#endif

// ----------------------------------------------------------------------------------------------------

#if defined(DIGITAL_INPUT_PINS)
    PROGMEM const uint8_t button_pin[] = {DIGITAL_INPUT_PINS,0}; // A single byte array causes a problem, so appending a 0 byte as workaround. 
    const uint8_t button_pin_count = (uint8_t)(MAX_INPUTS < sizeof(button_pin) ? MAX_INPUTS : sizeof(button_pin))-1;
#elif
    PROGMEM const uint8_t button_pin[] = {};
    const uint8_t button_pin_count = 0;
#endif

// ----------------------------------------------------------------------------------------------------

void TimerIO::init()
{
    /* Set button pins to input */
    for (uint8_t i=0; i < button_pin_count; i++)
    {
        pinMode(pgm_read_byte(&button_pin[i]), INPUT);
    }

#if defined(AVR)

    uint8_t port;

    for (uint8_t i=0; i < button_pin_count; i++)
    {
        port = digitalPinToPort(pgm_read_byte(&button_pin[i]));
        BUTTON_PORT_IN_REG[i] = portInputRegister(port);
        BUTTON_BIT[i] = digitalPinToBitMask(pgm_read_byte(&button_pin[i]));
    }

    beeperPortbit = digitalPinToBitMask(TRIGGER_PIN);
    port = digitalPinToPort(TRIGGER_PIN);
    beeperPortReg = portOutputRegister(port);
#endif
    
    if (TRIGGER_ENABLED)
    {
        /* Set the buzzer pin to an output and turn the buzzer off */
        pinMode(TRIGGER_PIN, OUTPUT);
        writeBeeper(0, true);
    }
}

// ----------------------------------------------------------------------------------------------------

void TimerIO::initialize(bool beeperInverted, bool inputsInverted)
{
    setBeeperInverted(beeperInverted);
    setInputsInverted(inputsInverted);

    init();

#if defined(AVR)
    // piggy back on to timer0, which is already set to approx 1khz.
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
    beginTimer(1000);
#endif
}

// ----------------------------------------------------------------------------------------------------

void TimerIO::queueButton(uint8_t button)
{
    if (buttonBufferCount <= sizeof(buttonBuffer))
    {
        buttonBuffer[button_write_pos] = button;
        buttonBufferCount++;
        button_write_pos++;

        if (button_write_pos >= sizeof(buttonBuffer))
        {
            button_write_pos = 0;
        }
    }
}

// ----------------------------------------------------------------------------------------------------
uint8_t TimerIO::getButton()
{
    uint8_t button = 0;

    if (buttonBufferCount > 0)
    {
        button = buttonBuffer[button_read_pos];
        buttonBufferCount--;
        button_read_pos++;

        if (button_read_pos >= sizeof(buttonBuffer))
        {
            button_read_pos = 0;
        }
    }

    return button;
}

// ----------------------------------------------------------------------------------------------------

void TimerIO::enableInputByIdx(uint8_t pinIdx, bool enabled)
{
    if (pinIdx < MAX_INPUTS)
    {
        BUTTON_ENABLED[pinIdx] = enabled;
        buttonState[pinIdx] = 0;
        buttonPressTime[pinIdx] = 0;
    }
}

void TimerIO::enableInputByPin(int8_t pin, bool enabled)
{
    for (uint8_t idx=0; idx < button_pin_count; idx++)
    {
        if (pin == pgm_read_byte(&button_pin[idx]))
        {
            enableInputByIdx(idx, enabled);
            break;
        }
    }
}

int8_t TimerIO::getInputPinByIdx(uint8_t pinIdx) const
{
    if (pinIdx < button_pin_count)
    {
        return pgm_read_byte(&button_pin[pinIdx]);
    }
    return -1;
}

void TimerIO::setInputsEnabled(bool enabled)
{
    for (uint8_t i=0; i<MAX_INPUTS; i++)
    {
        BUTTON_ENABLED[i] = enabled;

        buttonState[i] = 0;
        buttonPressTime[i] = 0;
    }
}

void TimerIO::invertInputByIdx(uint8_t pinIdx, bool inverted)
{
    if (pinIdx < button_pin_count)
    {
        buttonInverted[pinIdx] = inverted ? 1 : 0;
        pinMode(pgm_read_byte(&button_pin[pinIdx]), inverted ? INPUT_PULLUP : INPUT);
    }
}

void TimerIO::setInputsInverted(bool inverted)
{
    for (uint8_t i=0; i < button_pin_count; i++)
    {
        buttonInverted[i] = inverted ? 1 : 0;
        pinMode(pgm_read_byte(&button_pin[i]), inverted ? INPUT_PULLUP : INPUT);
    }
}

// ----------------------------------------------------------------------------------------------------

void TimerIO::setUserInterrupt(void (* volatile userInterruptHandler)())
{
    noInterrupts();
    userInterrupt = userInterruptHandler;
    interrupts();
}

// ----------------------------------------------------------------------------------------------------
void TimerIO::beep(uint16_t onPeriod, uint16_t offPeriod, uint8_t cycles, uint16_t loopCycles, uint16_t loopDelayPeriod)
{
    cycles = cycles == 0 ? 1 : cycles;

    beeperModifyInProgress = true; // must do this first before changing volatile fields.

    if (onPeriod == 0)
    {
        // Turn off beeper.
        writeBeeper(0, true);
    }

    beeperState = 0;
    beeperOnPeriodReloadValue = onPeriod * 10;
    beeperOffPeriodReloadValue = offPeriod * 10;
    beeperPeriodCounter = onPeriod * 10;

    beeperCycleReloadValue = cycles;
    beeperCycleCounter = cycles;

    beeperLoopCycleCounter = loopCycles;
    beeperLoopDelayPeriodReloadValue = loopDelayPeriod * 10;

    beeperModifyInProgress = false; // must do this last.
}

// ----------------------------------------------------------------------------------------------------
void TimerIO::setBeepOffPeriod(uint16_t offPeriod)
{
    beeperModifyInProgress = true; // must do this first before changing volatile fields.

    if (beeperState == 1)
    {
        if (offPeriod * 10 < beeperPeriodCounter)
        {
            beeperPeriodCounter = offPeriod * 10;
        }
    }

    beeperOffPeriodReloadValue = offPeriod * 10;
    beeperModifyInProgress = false; // must do this last.
}

// ----------------------------------------------------------------------------------------------------

void TimerIO::setBeeperInverted(bool inverted)
{
    beeperInverted = inverted ? 1 : 0;
}

// ----------------------------------------------------------------------------------------------------
void TimerIO::isrCallBack()
{
    // Beeper control.

    if (!beeperModifyInProgress && beeperOnPeriodReloadValue > 0)
    {
        if (beeperPeriodCounter == 0)
        {
            switch (beeperState)
            {
                case 0: // on period
                {
                    if (beeperOffPeriodReloadValue > 0)
                    {
                        beeperPeriodCounter = beeperOffPeriodReloadValue;
                        beeperState = 1;
                        break;
                    }
                    // Fall thru to next state immediately.
                }
                case 1: // off period
                {
                    beeperCycleCounter--;
                    if (beeperCycleCounter > 0)
                    {
                        beeperPeriodCounter = beeperOnPeriodReloadValue;
                        beeperState = 0;
                        break;
                    }
                    else
                    {
                        beeperCycleCounter = beeperCycleReloadValue;
                        beeperPeriodCounter = beeperLoopDelayPeriodReloadValue;
                        beeperState = 2;

                        if (beeperLoopDelayPeriodReloadValue > 0)
                        {
                            break;
                        }
                    }
                    // Fall thru to next state immediately.
                }
                case 2:                              // loop cycle delay period
                {
                    if (beeperLoopCycleCounter == 0) // loop beeper indefinitely
                    {
                        beeperPeriodCounter = beeperOnPeriodReloadValue;
                        beeperState = 0;
                    }
                    else
                    {
                        beeperLoopCycleCounter--;
                        if (beeperLoopCycleCounter == 0)
                        {
                            beeperOnPeriodReloadValue = 0; // beeper activity has now ended.
                            writeBeeper(0);
                        }
                        else
                        {
                            beeperPeriodCounter = beeperOnPeriodReloadValue;
                            beeperState = 0;
                        }
                    }
                    break;
                }
            }
        }

        if (beeperPeriodCounter > 0)
        {
            beeperPeriodCounter--;
        }

        if (beeperState == 0)
        {
            // beep on
            writeBeeper(1);
        }
        else
        {
            // beep off
            writeBeeper(0);
        }
    }

    // Bump button sample interval counter

    if (buttonSampleIntervalCounter++ >= BUTTON_SAMPLE_INTERVAL)
    {
        buttonSampleIntervalCounter = 0;

        uint8_t btnStateNow;

        for (uint8_t i = 0; i < button_pin_count; i++)
        {
            if (!BUTTON_ENABLED[i])
                continue;

#if defined(AVR)
            //btnStateNow = (!!(*portInputRegister(BUTTON_PORT[i]) & BUTTON_BIT[i])) ^ buttonInverted[i];
            btnStateNow = (!!(*(BUTTON_PORT_IN_REG[i]) & BUTTON_BIT[i])) ^ buttonInverted[i];
#else
            btnStateNow = digitalRead(pgm_read_byte(&button_pin[i])) ^ buttonInverted[i];
#endif
            // If button state has changed, action the change.
            if (buttonState[i] != btnStateNow)
            {
                // if button state changes to pressed, queue SHORT PRESS to buffer.
                if (btnStateNow)
                {
                    queueButton((i + 1) | BUTTON_PRESSED_IND);
                }
                else
                {
                    // otherwise button state has changed to up, queue SHORT or LONG RELEASE state to buffer, and reset pressed time counter.
                    if (buttonPressTime[i] > (BUTTON_REPEAT_THRESHOLD))
                    {
                        queueButton((i + 1) | BUTTON_LONG_RELEASE_IND);
                    }
                    else
                    {
                        queueButton((i + 1) | BUTTON_SHORT_RELEASE_IND);
                    }
                    buttonPressTime[i] = 0;
                }
                buttonState[i] = btnStateNow;
            }

            // if button state pressed, increment pressed time counter. Queue LONG PRESS to buffer, if button is held long.
            if (btnStateNow)
            {
                if (buttonPressTime[i] > (BUTTON_REPEAT_THRESHOLD) && (buttonPressTime[i] & 15) == 0)
                {
                    queueButton((i + 1) | BUTTON_LONG_PRESSED_IND);
                    buttonPressTime[i] = BUTTON_REPEAT_THRESHOLD;
                }

                //if (buttonPressTime[i] < 65535)
                //{
                    buttonPressTime[i]++;
                //}
            }
        }
    }
    writeBeeperOsc();
    if (userInterrupt)
    {
        userInterrupt();
    }
}

/* ---------------------------------------------------------------------- */

void TimerIO::writeBeeper(uint8_t value, bool force)
{
    if (TRIGGER_ENABLED)
    {
        bool valueModified = false;

        if (value > 0 && (force || !beeperEngaged))
        {
            beeperEngaged = true;
            valueModified = true;
        }
        else if (value == 0 && (force || beeperEngaged))
        {
            beeperEngaged = false;
            valueModified = true;
        }

        if (valueModified)
        {
#if defined(AVR)
            value = value ^ beeperInverted;
            writePortAVR(beeperPortReg, beeperPortbit, value);
#else
            digitalWrite(TRIGGER_PIN, value ^ beeperInverted);
#endif
        }
    }
}

void TimerIO::writeBeeperOsc()
{
    if (TRIGGER_ENABLED && TRIGGER_SIG_OSC_ENABLED)
    {
        uint8_t value = 0;

        if (beeperEngaged)
        {
            if (beeperSignalOsc > 0)
            {
                value = 1;
                beeperSignalOsc--;
            }
            else
            {
                value = 0;
                beeperSignalOsc = 1;
            }

#if defined(AVR)
            value = value ^ beeperInverted;
            writePortAVR(beeperPortReg, beeperPortbit, value);
#else
            digitalWrite(TRIGGER_PIN, value ^ beeperInverted);
#endif
        }
    }
}
// ----------------------------------------------------------------------------------------------------

#if defined(AVR)

SIGNAL(TIMER0_COMPA_vect)
{
    TmrIO.isrCallBack();
}

#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)

void timer_callback(timer_callback_args_t __attribute((unused)) * p_args)
{
    TmrIO.isrCallBack();
}

bool beginTimer(float rate)
{
    uint8_t timer_type = GPT_TIMER;
    int8_t tindex = FspTimer::get_available_timer(timer_type);
    if (tindex < 0)
    {
        tindex = FspTimer::get_available_timer(timer_type, true);
    }
    if (tindex < 0)
    {
        return false;
    }

    FspTimer::force_use_of_pwm_reserved_timer();

    if (r4Timer.begin(TIMER_MODE_PERIODIC, timer_type, tindex, rate, 0.0f, timer_callback))
        if (r4Timer.setup_overflow_irq())
            if (r4Timer.open())
                if (r4Timer.start())
                    return true;

    return false;
}
#endif
