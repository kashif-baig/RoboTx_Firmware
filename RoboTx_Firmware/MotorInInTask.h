#ifndef MOTOR_ININ_TASK_H
#define MOTOR_ININ_TASK_H

#include "IMotorTask.h"
// #include "Settings.h"

/**
 * Implements a motor task that uses an in/in motor controller.
 */
class MotorInInTask : public IMotorTask
{
    const uint8_t Motor2_In1; //  = 13;  // Left motor fwd
    const uint8_t Motor2_In2; //  = 12;  // Left motor rev
    const uint8_t Motor1_In3; //  = 11;  // Right motor rev
    const uint8_t Motor1_In4; //  = 10;  // Right motor fwd

    const uint8_t _motor_Countdown_Reload = 20;

    volatile uint8_t _motor1_mod = 0;
    volatile int8_t _motor1_PWM = 0;
    int8_t _motor1_Duty = 0;
    bool _motor1_On_Period = false;
    volatile bool _motor1_fwd = true;

    volatile uint8_t _motor2_mod = 0;
    volatile int8_t _motor2_PWM = 0;
    int8_t _motor2_Duty = 0;
    bool _motor2_On_Period = false;
    volatile bool _motor2_fwd = true;

    uint8_t _motor_Countdown = _motor_Countdown_Reload;
    uint8_t _mod_counter = 0;

    volatile bool _motor_updating = false;

public:
    MotorInInTask(uint8_t m1_fwd_pin, uint8_t m1_rev_pin, uint8_t m2_fwd_pin, uint8_t m2_rev_pin)
    : Motor1_In4(m1_fwd_pin), Motor1_In3(m1_rev_pin), Motor2_In1(m2_fwd_pin), Motor2_In2(m2_rev_pin)
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

                if (_motor1_PWM != 0)
                {
                    digitalWriteAlt(_motor1_fwd ? Motor1_In4 : Motor1_In3, HIGH);
                }
                if (_motor2_PWM != 0)
                {
                    digitalWriteAlt(_motor2_fwd ? Motor2_In1 : Motor2_In2, HIGH);
                }
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
                    if (_motor1_PWM != 0)
                    {
                        digitalWriteAlt(_motor1_fwd ? Motor1_In4 : Motor1_In3, LOW);
                    }
                    _motor1_On_Period = false;
                }
            }
            _motor1_Duty++;

            if (_motor2_Duty >= _motor2_PWM)
            {
                // Turn off motor if PWM < 20
                if (_motor2_On_Period && _motor2_PWM < _motor_Countdown_Reload)
                {
                    if (_motor2_PWM != 0)
                    {
                        digitalWriteAlt(_motor2_fwd ? Motor2_In1 : Motor2_In2, LOW);
                    }
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
        pinMode(Motor2_In1, OUTPUT);
        pinMode(Motor2_In2, OUTPUT);
        pinMode(Motor1_In3, OUTPUT);
        pinMode(Motor1_In4, OUTPUT);

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
        //int8_t pwm = (((int16_t)constrain(speedPercent + 2, -100, 100)) * 13) >> 6;
        int8_t pwm = constrain(speedPercent, -100, 100) / 5;
        int8_t speedPercentTmp = speedPercent;

        if (speedPercent < 0)
        {
            pwm = -pwm;
            speedPercentTmp = -speedPercentTmp;
        }

        int8_t modTmp = speedPercentTmp - (pwm * 5);

        _motor_updating = true;
        if (motor == Motor1)
        {
            _motor1_fwd = speedPercent >= 0;
            _motor1_mod = modTmp;
            _motor1_PWM = pwm;
            // set non pwm pin to 0.
            digitalWriteAlt(_motor1_fwd ? Motor1_In3 : Motor1_In4, speedPercent != 0 ? LOW : (MOTOR1_ININ_IDLE_ENABLED ? LOW : HIGH));

            if (speedPercent == 0)
            {
                // set pwm pin to 0.
                digitalWriteAlt(_motor1_fwd ? Motor1_In4 : Motor1_In3, MOTOR1_ININ_IDLE_ENABLED ? LOW : HIGH);
            }
        }
        else
        {
            _motor2_fwd = speedPercent >= 0;
            _motor2_mod = modTmp;
            _motor2_PWM = pwm;
            // set non pwm pin to 0.
            digitalWriteAlt(_motor2_fwd ? Motor2_In2 : Motor2_In1, speedPercent != 0 ? LOW : (MOTOR2_ININ_IDLE_ENABLED ? LOW : HIGH));

            if (speedPercent == 0)
            {
                // set pwm pin to 0.
                digitalWriteAlt(_motor2_fwd ? Motor2_In1 : Motor2_In2, MOTOR2_ININ_IDLE_ENABLED ? LOW : HIGH);
            }
        }
        setIsDriving(_motor1_PWM || _motor2_PWM);
        _motor_updating = false;
    }
};

#endif
