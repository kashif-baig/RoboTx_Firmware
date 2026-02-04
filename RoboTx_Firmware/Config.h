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
#if DEFAULT_MOTOR_DRIVER == MOTOR_DRIVER_DIRPWM
    sizeof(MotorDirPwmTask) ;
#elif DEFAULT_MOTOR_DRIVER == MOTOR_DRIVER_ININ
    sizeof(MotorInInTask);
#else
    sizeof(MotorHW56Task);
#endif
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

    Config(Display7SegTask *displayTask, TimerIO *timerIO, PulseCounterTask *pulseCounterTask, AnalogTask *analogTask);
    /**
     * Sets seven segment display to Left to Right.
     */
    void setDisplayLTR();

    /**
     * Sets seven segment display to Right to Left.
     */
    void setDisplayRTL();

    /**
     * Set the type seven segment LED display. Digits are shown in inverse
     * if the wrong type is selected.
     */
    void setDisplayInverse(bool isInverse);


    /**
     * To use inverted (low) signal for triggering beeper, set to true.
     * Set to false to use a high signal. 
     */
    void setBeeperInverted(bool inverted);

    /**
     * Returns true if an instance of a motor task is available
     * using method getMotorTask().
     */
    bool motorTaskAvailable() const;

    /**
     * Returns a motor task. Check if available first by using method motorTaskAvailable().
     */
    IMotorTask *getMotorTask() const;

    /**
     * Detect digital input (analog pin A0 to A4) on either high or low (inverted) signal.
     */
    void invertDigitalInput(uint8_t analogPinIdx, bool inverted);

    /**
     * Enables/disables digital input (analog pin A0 to A4) (see TimerIO.h).
     * Disables same pin for analog input.
     * If the pin is in use by I2C, then pin is not enabled.
     */
    void enableDigitalInput(uint8_t pinIdx, bool enabled);
    
    /**
     * Enables/disables analog input specified by index for analog pin.
     * Disables same pin for digital input.
     * If the pin is in use by I2C, then pin is not enabled.
     */
    void enableAnalogInput(uint8_t analogPinIdx, bool enabled);

    /**
     * Sets the state of I2C pins. If the MCU is an Uno, pin A4 and A5 are no longer available
     * for digital or analog input.
     */
    void setI2CInUse(bool inUse);

    /**
     * Uses MotorDirPwmTask which allows 2 motors to be driven, using 2 Arduino
     * pins per motor. One pin is for PWM, the other for direction.
     */
    void useMotorDriverDirPwm(bool useEnable);

    /**
     * Uses MotorInInTask which allows 2 motors to be driven, using 2 Arduino
     * pins per motor.
     */
    void useMotorDriverInIn();

   
    /**
     * Uses MotorHW56Task which allows control of 2 motors, using 3 Arduino
     * pins per motor. One pin for PWM, two pins for motor direction.
     */
    void useMotorDriverHW56();

    /**
     * Returns true if the I2C is in use, false otherwise.
     */
    bool i2CInUse () const;

    /**
     * Returns true if the MCU is an Uno, false otherwise.
     */
    bool mcuIsUno() const;

    /**
     * Returns true if the MCU is a Leonardo, false otherwise.
     */
    bool mcuIsLeonardo() const;

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