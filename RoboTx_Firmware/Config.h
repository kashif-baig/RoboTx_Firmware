#ifndef CONFIG_H
#define CONFIG_H

#include "TimerIO.h"
#include "Display7SegTask.h"
#include "MotorDirPwmTask.h"
#include "MotorInInTask.h"
#include "MotorHW56Task.h"
#include "IMotorTask.h"
#include "PulseCounterTask.h"
#include "AnalogTask.h"
#include "Settings.h"
#include <new>

#ifndef STATIC_MAX
  #define STATIC_MAX(a, b)  ((a) > (b) ? (a) : (b))
#endif

// Determine amount of memory to allocate for motor tasks.
static const size_t LARGEST_MOTORTASK_SIZE =
    STATIC_MAX(sizeof(MotorDirPwmTask),
    STATIC_MAX(sizeof(MotorInInTask),
    STATIC_MAX(sizeof(MotorHW56Task),
    0)));

// Pre-allocated memory for motor tasks. Align to 4 byte boundary.

union
{
    char Memory[LARGEST_MOTORTASK_SIZE];
    uint32_t dummy;
} MotorTaskUnion;

static char* MotorTask = MotorTaskUnion.Memory;

/**
 * Global configuration settings.
 */
class Config
{
public:

    Config(Display7SegTask *displayTask, TimerIO *timerIO, PulseCounterTask *pulseCounterTask, AnalogTask *analogTask)
    {
        _timerIO = timerIO;
        _display7SegTask = displayTask;
        _pulseCounterTask = pulseCounterTask;
        _analogTask = analogTask;
    }

    /**
     * Sets seven segment display to Left to Right.
     */
    void setDisplayLTR()
    {
        _display7SegTask->setSegmentRTL(false);
    }

    /**
     * Sets seven segment display to Right to Left.
     */
    void setDisplayRTL()
    {
        _display7SegTask->setSegmentRTL(true);
    }

    /**
     * Set the type seven segment LED display. Digits are shown in inverse
     * if the wrong type is selected.
     */
    void setDisplayInverse(bool isInverse)
    {
        _display7SegTask->setDisplayInverse(isInverse);
    }


    /**
     * To use inverted (low) signal for triggering beeper, set to true.
     * Set to false to use a high signal. 
     */
    void setBeeperInverted(bool inverted)
    {
        _timerIO->setBeeperInverted(inverted);
    }

    /**
     * Returns true if an instance of a motor task is available
     * using method getMotorTask().
     */
    bool motorTaskAvailable() const
    {
        return _motorTaskAvailable;
    }

    /**
     * Returns a motor task. Check if available first by using method motorTaskAvailable().
     */
    IMotorTask *getMotorTask() const
    {
        return _motorTask;
    }

    /**
     * Detect digital input (analog pin A0 to A4) on either high or low (inverted) signal.
     */
    void invertDigitalInput(uint8_t analogPinIdx, bool inverted)
    {
        _timerIO->invertInputByIdx(analogPinIdx, inverted);
    }

    /**
     * Enables/disables digital input (analog pin A0 to A4) (see TimerIO.h).
     * Disables same pin for analog input.
     * If the pin is in use by I2C, then pin is not enabled.
     */
    void enableDigitalInput(uint8_t pinIdx, bool enabled)
    {
        int8_t pin = _timerIO->getInputPinByIdx(pinIdx);

        if (enabled)
        {
            // disable same pin if used for analog reading.
            _analogTask->enableInputByPin(pin, false);

            if (pin == _pulseCounterTask->getPulseInPin())
            {
                // Disable pulse counter.
                _pulseCounterTask->disable();
            }
        }
        _timerIO->enableInputByIdx(pinIdx, enabled && !((pin == A4 || pin == A5) && i2CInUse() && mcuIsUno()));
    }
    
    /**
     * Enables/disables analog input specified by index for analog pin.
     * Disables same pin for digital input.
     * If the pin is in use by I2C, then pin is not enabled.
     */
    void enableAnalogInput(uint8_t analogPinIdx, bool enabled)
    {
        if (enabled)
        {
            int8_t pin = _analogTask->getInputPinByIdx(analogPinIdx);

            // disable same pin for digital reading.
            //_timerIO->enableInputByPin(A0 + analogPinIdx, false);
            _timerIO->enableInputByPin(pin, false);

            //if ((A0 + analogPinIdx) == _pulseCounterTask->getPulseInPin())
            if (pin == _pulseCounterTask->getPulseInPin())
            {
                // Disable pulse counter.
                _pulseCounterTask->disable();
            }
        }
        _analogTask->enableInputByIdx(analogPinIdx, enabled  && !((analogPinIdx == 4 || analogPinIdx == 5) && i2CInUse() && mcuIsUno()));
    }

    /**
     * Sets the state of I2C pins. If the MCU is an Uno, pin A4 and A5 are no longer available
     * for digital or analog input.
     */
    void setI2CInUse(bool inUse)
    {
        _i2CInUse = inUse;

        if (inUse && mcuIsUno())
        {
            // disable Uno I2C pin A4,A5 for analog/digital reading.
            _analogTask->enableInputByIdx(4, false);
            _analogTask->enableInputByIdx(5, false);

            _timerIO->enableInputByPin(A4, false);
            _timerIO->enableInputByPin(A5, false);
        }
        if (onSettingI2CInUse != NULL)
        {
            onSettingI2CInUse();
        }
    }


    /**
     * Uses MotorDirPwmTask which allows 2 motors to be driven, using 2 Arduino
     * pins per motor. One pin is for PWM, the other for direction.
     */
    void useMotorDriverDirPwm(bool useEnable)
    {
        if (_motorTask != NULL)
        {
            // Disable existing task.
            _motorTask->disable();
        }
        _motorTaskAvailable = false;
        _motorTask = new (MotorTask) MotorDirPwmTask(useEnable, MOTOR1_DIRPWM_PWM_PIN, MOTOR1_DIRPWM_DIR_PIN, MOTOR2_DIRPWM_PWM_PIN, MOTOR2_DIRPWM_DIR_PIN);
        _motorTask->disable();
        _motorTaskAvailable = true;
    }

    /**
     * Uses MotorInInTask which allows 2 motors to be driven, using 2 Arduino
     * pins per motor.
     */
    void useMotorDriverInIn(/*bool useEnable*/)
    {
        if (_motorTask != NULL)
        {
            // Disable existing task.
            _motorTask->disable();
        }
        _motorTaskAvailable = false;
        _motorTask = new (MotorTask) MotorInInTask(MOTOR1_ININ_FWD_PIN, MOTOR1_ININ_REV_PIN, MOTOR2_ININ_FWD_PIN, MOTOR2_ININ_REV_PIN);
        _motorTask->disable();
        _motorTaskAvailable = true;
    }

   
    /**
     * Uses MotorHW56Task which allows control of 2 motors, using 3 Arduino
     * pins per motor. One pin for PWM, two pins for motor direction.
     */
    void useMotorDriverHW56()
    {
        if (_motorTask != NULL)
        {
            _motorTask->disable();
        }
        _motorTaskAvailable = false;
        _motorTask = new (MotorTask) MotorHW56Task(MOTOR1_HW56_DIR1_PIN, MOTOR1_HW56_DIR2_PIN, MOTOR2_HW56_DIR1_PIN, MOTOR2_HW56_DIR2_PIN);
        _motorTaskAvailable = true;
    }

    /**
     * Returns true if the I2C is in use, false otherwise.
     */
    bool i2CInUse () const
    {
        return _i2CInUse;
    }


    /**
     * Returns true if the MCU is an Uno, false otherwise.
     */
    bool mcuIsUno() const
    {
#if defined(__AVR_ATmega328P__) || defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
        return true;
#else
        return false;
#endif
    }

    /**
     * Returns true if the MCU is a Leonardo, false otherwise.
     */
    bool mcuIsLeonardo() const
    {
#if defined(__AVR_ATmega32U4__)
        return true;
#else
        return false;
#endif
    }

    // Callback invoked when setting I2C in use.
    void (* onSettingI2CInUse)() = 0;

private:
    TimerIO *_timerIO;
    Display7SegTask *_display7SegTask;
    IMotorTask * volatile _motorTask = NULL;
    PulseCounterTask *_pulseCounterTask;
    AnalogTask *_analogTask;
    volatile bool _motorTaskAvailable = false;
    
    bool _i2CInUse = false;
};

#endif