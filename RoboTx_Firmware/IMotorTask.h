#ifndef IMOTOR_TASK_H
#define IMOTOR_TASK_H

#include "Task.h"
#include "MotorSpeedController.h"
#include "Settings.h"
//#include "Debug.h"

#define MOTOR_TASK_ACCEL_INTERVAL_MS 50
#define MOTOR_TASK_INFINITE_DURATION -1

#ifndef MOTOR_TASK_DISABLE_ON_BOOT
    #define MOTOR_TASK_DISABLE_ON_BOOT  false
#endif

#ifndef MOTOR1_ININ_IDLE_ENABLED
    #define MOTOR1_ININ_IDLE_ENABLED   true    /* Set to true to enable idling/coasting */
#endif
#ifndef MOTOR1_ININ_FWD_PIN
    #define MOTOR1_ININ_FWD_PIN        10
#endif
#ifndef MOTOR1_ININ_REV_PIN
    #define MOTOR1_ININ_REV_PIN        11
#endif
#ifndef MOTOR2_ININ_IDLE_ENABLED
    #define MOTOR2_ININ_IDLE_ENABLED   true    /* Set to true to enable idling/coasting */
#endif
#ifndef MOTOR2_ININ_FWD_PIN
    #define MOTOR2_ININ_FWD_PIN        13
#endif
#ifndef MOTOR2_ININ_REV_PIN
    #define MOTOR2_ININ_REV_PIN        12
#endif

#ifndef MOTOR1_DIRPWM_PWM_PIN
    #define MOTOR1_DIRPWM_PWM_PIN      11
#endif
#ifndef MOTOR1_DIRPWM_DIR_PIN
    #define MOTOR1_DIRPWM_DIR_PIN      10
#endif
#ifndef MOTOR2_DIRPWM_PWM_PIN
    #define MOTOR2_DIRPWM_PWM_PIN      12
#endif
#ifndef MOTOR2_DIRPWM_DIR_PIN
    #define MOTOR2_DIRPWM_DIR_PIN      13
#endif
#ifndef MOTOR1_DIRPWM_INV_DIR
    #define MOTOR1_DIRPWM_INV_DIR      false   /* Set to true to invert motor 1 direction */
#endif
#ifndef MOTOR2_DIRPWM_INV_DIR
    #define MOTOR2_DIRPWM_INV_DIR      false   /* Set to true to invert motor 2 direction */
#endif
#ifndef MOTOR_DIRPWM_EN_DEFAULT
    #define MOTOR_DIRPWM_EN_DEFAULT    false   /* Set to true to use motor enable pins by default */
#endif
#ifndef MOTOR_DIRPWM_ENABLE_VALUE
    #define MOTOR_DIRPWM_ENABLE_VALUE  HIGH    /* value to use to enable a motor */
#endif
#ifndef MOTOR1_DIRPWM_EN_PIN
    #define MOTOR1_DIRPWM_EN_PIN       -1      /* Assigning an enable pin only for Motor1 means it can act as master enable for both motors. */
#endif
#ifndef MOTOR2_DIRPWM_EN_PIN
    #define MOTOR2_DIRPWM_EN_PIN       -1      /* -1 means unassigned */
#endif

#ifndef MOTOR1_HW56_INV_DIR
    #define MOTOR1_HW56_INV_DIR false
#endif
#ifndef MOTOR1_HW56_IDLE_ENABLED
    #define MOTOR1_HW56_IDLE_ENABLED true
#endif
#ifndef MOTOR1_HW56_DIR1_PIN
    #define MOTOR1_HW56_DIR1_PIN  -1
#endif
#ifndef MOTOR1_HW56_DIR2_PIN
    #define MOTOR1_HW56_DIR2_PIN  -1
#endif

#ifndef MOTOR2_HW56_IDLE_ENABLED
    #define MOTOR2_HW56_IDLE_ENABLED true
#endif
#ifndef MOTOR2_HW56_INV_DIR
    #define MOTOR2_HW56_INV_DIR false
#endif
#ifndef MOTOR2_HW56_DIR1_PIN
    #define MOTOR2_HW56_DIR1_PIN  -1
#endif
#ifndef MOTOR2_HW56_DIR2_PIN
    #define MOTOR2_HW56_DIR2_PIN  -1
#endif


/**
 * Defines motor IDs.
 */
enum MotorId : uint8_t
{
    Motor1 = 1,
    Motor2 = 2
};

/**
 * MotorTask FSM state.
 */
enum MotorTaskState : uint8_t
{
    MotorTaskState_Inactive,
    MotorTaskState_Enabled,
    MotorTaskState_Disabled
};

/**
 * Abstract base class for motor driving tasks. The initial state of the motor task is inactive.
 * This allows the Arduino motor pins to be used by other devices, as long as the motor task
 * remains in an inactive state.
 */
class IMotorTask : public Task
{
    const uint32_t _durationMs = 500;

    bool _isDriving = false;
    volatile MotorTaskState _taskState = MotorTaskState_Inactive;

    volatile bool _connectionIsAlive = false;

    MotorSpeedController _motor1_speed_controller;
    MotorSpeedController _motor2_speed_controller;

    // Gets the speed controller for a motor.
    // If implementation has a different number of motors, convert to virtual method and override.
    MotorSpeedController* getSpeedController(uint8_t motor)
    {
        if (motor == Motor1) return &_motor1_speed_controller;
        if (motor == Motor2) return &_motor2_speed_controller;

        return NULL;
    }

protected:
    // Sets the motor driving state.
    void setIsDriving(bool isDriving)
    {
        _isDriving = isDriving;
    }

    // Returns true if any of the motors are currrently turning.
    bool isDriving() const
    {
        return _isDriving;
    }

    // Initializes the task and motor state.
    virtual void init() = 0;

    // Drives a motor with a speed specified as a +/- percentage.
    virtual void driveMotor(uint8_t motor, int8_t speedPercent) = 0;
    
public:
    // Returns the number of motors controlled by the task.
    // If implementation has a different number of motors, convert to virtual method and override.
    uint8_t getMotorCount() const { return 2; }

    // Performs interrupt service routine.
    virtual void doISR() = 0;

    // Sets the acceleration of the motor.
    void setAcceleration(uint8_t motor, int16_t accelStep)
    {
        getSpeedController(motor)->setAcceleration(accelStep);
    }

    // Set the motor target speed and whether or not to accelerate to it.
    void setTargetSpeed(uint8_t motor, int8_t speedPercent, bool overrideAcceleration)
    {
        MotorSpeedController *speedController = getSpeedController(motor);

        speedController->setTargetSpeed(speedPercent);

        // if acceleration is 0 (not enabled) or not required, drive motor immediately.
        if (!speedController->accelerationEnabled() || overrideAcceleration)
        {
            speedController->overrideAcceleration(speedPercent);
            driveMotor(motor, speedPercent);
        }
    }

    // Stops any acceleration and keeps the motor at the current speed.
    void stopAccelerating(uint8_t motor)
    {
        getSpeedController(motor)->stopAccelerating();
    }

    void setDuration(uint8_t motor, int16_t durationMs)
    {
        getSpeedController(motor)->setDuration(durationMs);
    }

    // Stops all motors.
    void allStop()
    {
        for (uint8_t i = 1; i <= getMotorCount(); i++)
        {
            setTargetSpeed(i, 0, true);
        }
    }

    // Disables the task.
    void disable()
    {
        if (_taskState != MotorTaskState_Disabled)
        {
            init();
        }
        _taskState = MotorTaskState_Disabled;
    }

    // Enables the task.
    void enable()
    {
        if (_taskState != MotorTaskState_Enabled)
        {
            init();
            for (uint8_t i=1; i <= getMotorCount(); i++)
            {
                getSpeedController(i)->init();
            }
            setTime();
        }
        _taskState = MotorTaskState_Enabled;
    }

    // Returns true if task is enabled.
    bool isEnabled() const
    {
        return _taskState == MotorTaskState_Enabled;
    }

    // Returns true if task is enabled and serial connection to PC is reported to be alive.
    bool isActive() const
    {
        return _taskState == MotorTaskState_Enabled && _connectionIsAlive;
    }

    // Inform this task if the serial connection to PC is alive or not.
    void informConnectionState(bool isAlive)
    {
        _connectionIsAlive = isAlive;
    }

    /**
     * Manage acceleration and stop motors on serial connection loss.
     */
    void doEvents()
    {
        if (isActive())
        {
            // On 50ms interval, run acceleration logic.
            uint16_t timeDiff = compareTime();

            if (timeDiff >= MOTOR_TASK_ACCEL_INTERVAL_MS)
            {
                // Set task timer and minimise time drift.
                setTime(timeDiff - MOTOR_TASK_ACCEL_INTERVAL_MS);

                // For each motor, if acceleration is enabled calculate new speed and drive motor.
                for (uint8_t i=1; i <= getMotorCount(); i++)
                {
                    MotorSpeedController *speedController = getSpeedController(i);

                    if (speedController->accelerationEnabled() && speedController->isAccelerating())
                    {
                        int8_t newSpeed = speedController->calculateNewSpeed();
                        driveMotor(i, newSpeed);
                        //Debug.println(newSpeed);
                    }

                    if (speedController->durationExpired())
                    {
                        // If a duration has been set and has expired, stop the motor.
                        setTargetSpeed(i, 0, true);
                    }
                }
            }
        }
        else if (isDriving())
        {
            // Stop motors from continuously running in the event the connection is lost.
            allStop();
        }
    }
};

#endif
