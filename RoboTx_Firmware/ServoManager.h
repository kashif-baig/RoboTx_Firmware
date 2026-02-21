#ifndef SERVO_MANAGER_H
#define SERVO_MANAGER_H

#include <Servo.h>
#include "Settings.h"

#define SV_DEFAULT_MIN_PULSE_WIDTH       544     // the default shortest pulse sent to a servo
#define SV_DEFAULT_MAX_PULSE_WIDTH      2400     // the default longest pulse sent to a servo

#define SV_MAX_SPEEDSTEP    50

/**
 * Manages an individual servo.
 */
class ManagedServo
{
protected:
    Servo _servo;
    uint8_t _pin;
    int8_t _speedStep = SV_MAX_SPEEDSTEP;
    int8_t _direction = 0;

    int16_t _targetPulse = 0;
public:

    // Sets the pin used for the servo.
    void setPin(uint8_t pin)
    {
        _pin = pin;
    }

    // Sets the speed step for the servo.
    void setSpeedStep(uint8_t speedStep)
    {
        _speedStep = constrain(speedStep, 1, SV_MAX_SPEEDSTEP);
    }

    // Sets the range of in terms of the pulse length and activates the servo.
    void attach(int16_t min, int16_t max)
    {
        _servo.attach(_pin, min, max);
    }

    // Activates the servo.
    void attach()
    {
        _servo.attach(_pin);
    }

    // Writes the duration of the pulse sent to the servo.
    void writeMicroseconds(int16_t value)
    {
        int16_t currentPos = _servo.readMicroseconds();

        if (_speedStep >= SV_MAX_SPEEDSTEP || _direction == 0)
        {
            _servo.writeMicroseconds(value);
            _targetPulse = 0;
            _direction = 1;
        }
        else
        {
            _targetPulse = value;
            _direction = value - currentPos >= 0 ? 1: -1;
        }
    }

    void updateMicroseconds()
    {
        if (_targetPulse && _speedStep < SV_MAX_SPEEDSTEP)
        {
            int16_t currentPos = _servo.readMicroseconds();

            currentPos += _speedStep * _direction;

            if ((_direction >= 0 && currentPos >= _targetPulse) || (_direction < 0 && currentPos <= _targetPulse))
            {
                _servo.writeMicroseconds(_targetPulse);
                _targetPulse = 0;
            }
            else
            {
                _servo.writeMicroseconds(currentPos);
            }
        }
    }

    // De-activates the servo.
    void detach()
    {
        _servo.detach();
    }

    // Returns true if the servo is activated.
    bool attached()
    {
        return _servo.attached();
    }
};

/**
 * Centralized management of one or more servo motors.
 * The initial state of a servo is inactive. The Arduino pin associated with a
 * servo can be used by other devices as long as the ServoManager has
 * not placed the servo on active duty by enabling it.
 */
class ServoManager
{
#if (SERVO1_ENABLED == true)
    ManagedServo _servo1;
#endif
#if (SERVO2_ENABLED == true)
    ManagedServo _servo2;
#endif
#if (SERVO3_ENABLED == true)
    ManagedServo _servo3;
#endif
#if (SERVO4_ENABLED == true)
    ManagedServo _servo4;
#endif

public:
    ServoManager(uint8_t servo1Pin, uint8_t servo2Pin, uint8_t servo3Pin, uint8_t servo4Pin)
    {
        setTime();
#if (SERVO1_ENABLED == true)
        {
            _servo1.setPin(servo1Pin);
        }
#endif
#if (SERVO2_ENABLED == true)
        {
            _servo2.setPin(servo2Pin);
        }
#endif
#if (SERVO3_ENABLED == true)
        {
            _servo3.setPin(servo3Pin);
        }
#endif
#if (SERVO4_ENABLED == true)
        {
            _servo4.setPin(servo4Pin);
        }
#endif
    }

    /**
     * Sets the servo range and activates the servo.
     */
    void setServoRange(uint8_t servoNumber, int16_t min, int16_t max)
    {
        min = constrain(min, 450, 1050);
        max = constrain(max, min + 1, 2550);

        ManagedServo *servo;
        if (servo = getServo(servoNumber))
        {
            servo->attach(min, max);
        }
    }

    /**
     * Sets the speedStep (increment/decrement) by which the servo moves to its next position.
     * The speed step is expressed as microseconds that are added to the servo pulse width value.
     */
    void setServoSpeedStep(uint8_t servoNumber, uint8_t speedStep)
    {
        ManagedServo *servo;
        if (servo = getServo(servoNumber))
        {
            servo->setSpeedStep(speedStep);
        }
    }

    /**
     * Stops the servo maintaining its position.
     */
    void disableServo(uint8_t servoNumber)
    {
        ManagedServo *servo;
        if (servo = getServo(servoNumber))
        {
            return servo->detach();
        }
    }

    /**
     * Sets the servo position expressed as pulse width in microseconds.
     */
    void setServoPulse(uint8_t servoNumber, int16_t pulseWidth)
    {
        if (!servoEnabled(servoNumber))
        {
            enableServo(servoNumber);
        }
        ManagedServo *servo;
        if (servo = getServo(servoNumber))
        {
            return servo->writeMicroseconds(pulseWidth);
        }
    }

    /**
     * Updates servo positions in an incremental manner as a way to control the servo speed.
     */
    void doEvents()
    {
        uint32_t timeDiff = compareTime();

        // Check if interval lapsed.
        if (timeDiff >= 4000)
        {
            // Set task timer and minimise time drift.
            setTime(timeDiff - 4000);

            ManagedServo *servo;
            for (uint8_t i=1; i<=4; i++)
            {
                if (servo = getServo(i))
                {
                    servo->updateMicroseconds();
                }
            }
        }
    }

private:

    void enableServo(uint8_t servoNumber)
    {
        ManagedServo *servo;
        if (servo = getServo(servoNumber))
        {
            servo->attach();
        }
    }

    bool servoEnabled(uint8_t servoNumber)
    {
        ManagedServo *servo;
        if (servo = getServo(servoNumber))
        {
            return servo->attached();
        }
        return false;
    }
    
    ManagedServo *getServo(uint8_t servoNumber)
    {
        switch (servoNumber)
        {
            case 1:
            {
#if (SERVO1_ENABLED == true)
                return &_servo1;
#endif
                break;
            }
            case 2:
            {
#if (SERVO2_ENABLED == true)
                return &_servo2;
#endif
                break;
            }
            case 3:
            {
#if (SERVO3_ENABLED == true)
                return &_servo3;
#endif
                break;
            }
            case 4:
            {
#if (SERVO4_ENABLED == true)
                return &_servo4;
#endif
                break;
            }
        }
        return NULL;
    }

    // Records the current value of the microsecond counter, minus
    // an optional offset.
    void setTime(uint16_t offsetUs = 0)
    {
        _timeUs = micros() - (int32_t)offsetUs;
    }

    // Returns the difference between the current value of the microsecond counter
    // and the recorded value.
    uint32_t compareTime()
    {
        return micros() - _timeUs;
    }

    uint32_t _timeUs;
};

#endif
