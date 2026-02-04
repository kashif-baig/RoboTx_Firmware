#ifndef MOTOR_DIRPWM_TASK_H
#define MOTOR_DIRPWM_TASK_H

#include "IMotorTask.h"

/**
 * Implements a motor task that uses a dir/pwm motor controller.
 */
class MotorDirPwmTask : public IMotorTask
{
    // Pin assignments
    const uint8_t Motor2_Dir; //  = 13;  // Left motor Direction 1
    const uint8_t Motor2_PWM; //  = 12;  // Left motor PWM speed control
    const uint8_t Motor1_PWM; //  = 11;  // Right motor PWM speed control
    const uint8_t Motor1_Dir; //  = 10;  // Right motor Direction 1
    const int8_t Enable1Pin;  // Master enable for 1st or all motors
    const int8_t Enable2Pin;  // Enable for 2nd motor (optional)

    const uint8_t _motor_Countdown_Reload = 20;

    volatile uint8_t _motor1_mod = 0;
    volatile int8_t _motor1_PWM = 0;
    int8_t _motor1_Duty = 0;
    bool _motor1_On_Period = false;

    volatile uint8_t _motor2_mod = 0;
    volatile int8_t _motor2_PWM = 0;
    int8_t _motor2_Duty = 0;
    bool _motor2_On_Period = false;

    uint8_t _motor_Countdown = _motor_Countdown_Reload;
    uint8_t _mod_counter = 0;

    volatile bool _motor_updating = false;

public:
    MotorDirPwmTask(bool useEnable, uint8_t m1_pwm_pin, uint8_t m1_dir_pin, uint8_t m2_pwm_pin, uint8_t m2_dir_pin)
    : Enable1Pin(useEnable ? MOTOR1_DIRPWM_EN_PIN : -1), Enable2Pin(useEnable ? MOTOR2_DIRPWM_EN_PIN : -1),
      Motor1_PWM(m1_pwm_pin), Motor1_Dir(m1_dir_pin), Motor2_PWM(m2_pwm_pin), Motor2_Dir(m2_dir_pin)
    {
    }


    /**
     * Motor PWM is managed by interrupt service routine.
     */
    void doISR()
    {
        if (isEnabled() && !_motor_updating)
        {
            if (_motor_Countdown)
            {
                _motor_Countdown--;
            }
            if (!_motor_Countdown)
            {
                _motor_Countdown = _motor_Countdown_Reload;

                _motor1_Duty = 0;
                _motor2_Duty = 0;

                // Turn on motor if PWM > 0
                digitalWriteAlt(Motor2_PWM, _motor2_PWM != 0 ? HIGH : LOW);
                digitalWriteAlt(Motor1_PWM, _motor1_PWM != 0 ? HIGH : LOW);

                _motor1_On_Period = true;
                _motor2_On_Period = true;

                // PWM control to slightly widen the duty pulse to deal with
                // speeds where speedPercent mod 5 is non zero.
                if (_mod_counter > 0)
                {
                    if (_motor1_PWM && _mod_counter <= _motor1_mod)
                    {
                        _motor1_Duty = -1;
                    }
                    if (_motor2_PWM && _mod_counter <= _motor2_mod)
                    {
                        _motor2_Duty = -1;
                    }
                }

                _mod_counter++;
                if (_mod_counter >= 5)
                {
                    _mod_counter = 0;
                }
            }

            if (_motor1_Duty >= _motor1_PWM)
            {
                // Turn off motor if PWM < 20
                if (_motor1_On_Period && _motor1_PWM < _motor_Countdown_Reload)
                {
                    digitalWriteAlt(Motor1_PWM, LOW);
                    _motor1_On_Period = false;
                }
            }
            _motor1_Duty++;

            if (_motor2_Duty >= _motor2_PWM)
            {
                // Turn off motor if PWM < 20
                if (_motor2_On_Period && _motor2_PWM < _motor_Countdown_Reload)
                {
                    digitalWriteAlt(Motor2_PWM, LOW);
                    _motor2_On_Period = false;
                }
            }

            _motor2_Duty++;
        }
    }

protected:
    // Initializes the task and motor state.
    void init()
    {
        IMotorTask::init();
        
        pinMode(Motor2_PWM, OUTPUT);
        pinMode(Motor2_Dir, OUTPUT);
        pinMode(Motor1_Dir, OUTPUT);
        pinMode(Motor1_PWM, OUTPUT);
        if (Enable1Pin > -1)
        {
            pinMode(Enable1Pin, OUTPUT);
        }
        if (Enable2Pin > -1)
        {
            pinMode(Enable2Pin, OUTPUT);
        }
        allStop();
    }

    // Drives a motor with a speed specified as a +/- percentage.
    void driveMotor(uint8_t motor, int8_t speedPercent)
    {
        if (motor < Motor1 || motor > Motor2)
        {
            return;
        }

        // Scale speed percent to actual +/- 20
        int8_t pwm = constrain(speedPercent, -100, 100) / 5;
        int8_t speedPercentTmp = speedPercent;

        if (speedPercent < 0)
        {
            pwm = -pwm;
            speedPercentTmp = -speedPercentTmp;
        }

        int8_t modTmp = speedPercentTmp - (pwm * 5);

        uint8_t directionVal = speedPercent > 0 ? HIGH : LOW;

        if (motor == Motor1)
        {
            digitalWrite(Motor1_Dir, !MOTOR1_DIRPWM_INV_DIR ? directionVal : !directionVal);
        }
        else
        {
            digitalWrite(Motor2_Dir, !MOTOR2_DIRPWM_INV_DIR ? directionVal : !directionVal);
        }

        _motor_updating = true;
        if (motor == Motor1)
        {
            _motor1_mod = modTmp;
            _motor1_PWM = pwm;
            if (speedPercent == 0)
            {
                digitalWriteAlt(Motor1_PWM, LOW);
                digitalWriteAlt(Motor1_Dir, LOW);
            }
        }
        else
        {
            _motor2_mod = modTmp;
            _motor2_PWM = pwm;
            if (speedPercent == 0)
            {
                digitalWriteAlt(Motor2_PWM, LOW);
                digitalWriteAlt(Motor2_Dir, LOW);
            }
        }
        setIsDriving(_motor1_PWM || _motor2_PWM);
        _motor_updating = false;

        uint8_t enableValue = MOTOR_DIRPWM_ENABLE_VALUE > 0 ? 1 : 0;

        if (Enable1Pin > -1 && Enable2Pin < 0)
        {
            digitalWrite(Enable1Pin, _motor1_PWM || _motor2_PWM ? enableValue : !enableValue);
        }
        else if (Enable1Pin > -1)
        {
            digitalWrite(Enable1Pin, _motor1_PWM ? enableValue : !enableValue);
            digitalWrite(Enable2Pin, _motor2_PWM ? enableValue : !enableValue);
        }
    }
};

#endif
