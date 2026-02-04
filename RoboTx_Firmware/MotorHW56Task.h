#ifndef MOTOR_HW56_TASK_H
#define MOTOR_HW56_TASK_H

#include "IMotorTask.h"

#define MOTOR1_HW56_PWM_PIN   5
#define MOTOR2_HW56_PWM_PIN   6

/**
 * Implements a motor task that uses a dir/pwm motor controller that uses pins 5 and 6 for hardware PWM.
 */
class MotorHW56Task : public IMotorTask
{
    // Pin assignments
    const int8_t Motor2_Dir1;
    const int8_t Motor2_Dir2;
    const int8_t Motor1_Dir1;
    const int8_t Motor1_Dir2;

    bool _motor1_driving = false;
    bool _motor2_driving = false;

public:
    MotorHW56Task(int8_t m1_dir1_pin, int8_t m1_dir2_pin, int8_t m2_dir1_pin, int8_t m2_dir2_pin)
    : Motor1_Dir1(m1_dir1_pin), Motor1_Dir2(m1_dir2_pin), Motor2_Dir1(m2_dir1_pin), Motor2_Dir2(m2_dir2_pin)
    {
    }

    void doISR()
    {
  
    }

protected:
    // Initializes the task and motor state.
    void init()
    {
        IMotorTask::init();

        if (Motor1_Dir1 > -1)
        {
            pinMode(Motor1_Dir1, OUTPUT);
        }
        if (Motor1_Dir2 > -1)
        {
            pinMode(Motor1_Dir2, OUTPUT);
        }

        if (Motor2_Dir1 > -1)
        {
            pinMode(Motor2_Dir1, OUTPUT);
        }
        if (Motor2_Dir2 > -1)
        {
            pinMode(Motor2_Dir2, OUTPUT);
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

        bool fwddir = speedPercent > 0;// ? 1 : 0;

        if (speedPercent < 0)
        {
            speedPercent = -speedPercent;
        }
        if (motor == Motor1 && MOTOR1_HW56_INV_DIR || motor == Motor2 && MOTOR2_HW56_INV_DIR)
        {
            fwddir = !fwddir;
        }

        // scale up percent to 255.
        uint8_t pwm = ((uint16_t)speedPercent * 327) / 128;

        analogWrite(motor == Motor1 ? MOTOR1_HW56_PWM_PIN : MOTOR2_HW56_PWM_PIN, pwm);

        if (motor == Motor1)
        {
            _motor1_driving = speedPercent!=0;

            if (Motor1_Dir1 > -1)
            {
                digitalWriteAlt(Motor1_Dir1, !MOTOR1_HW56_IDLE_ENABLED && !_motor1_driving ? 0 : (fwddir ? 0 : 1));

                if (Motor1_Dir2 > -1)
                {
                    digitalWriteAlt(Motor1_Dir2, !MOTOR1_HW56_IDLE_ENABLED && !_motor1_driving ? 0 : (fwddir ? 1 : 0));
                    
                    if(!MOTOR1_HW56_IDLE_ENABLED && !_motor1_driving)
                    {
                        digitalWriteAlt(MOTOR1_HW56_PWM_PIN, 1);
                    }
                }
            }
        }
        else
        {
            _motor2_driving = speedPercent!=0;

            if (Motor2_Dir1 > -1)
            {
                digitalWriteAlt(Motor2_Dir1, !MOTOR2_HW56_IDLE_ENABLED && !_motor2_driving ? 0 : (fwddir ? 0 : 1));

                if (Motor2_Dir2 > -1)
                {
                    digitalWriteAlt(Motor2_Dir2, !MOTOR2_HW56_IDLE_ENABLED && !_motor2_driving ? 0 : (fwddir ? 1 : 0));
                    
                    if(!MOTOR2_HW56_IDLE_ENABLED && !_motor2_driving)
                    {
                        digitalWriteAlt(MOTOR2_HW56_PWM_PIN, 1);
                    }
                }
            }
        }

        setIsDriving(_motor1_driving || _motor2_driving);
    }
};

#endif
