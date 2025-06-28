#ifndef MOTOR_SPEED_CONTROLLER_H
#define MOTOR_SPEED_CONTROLLER_H

/**
 * Used by IMotorTask to control motor acceleration and duration.
 */
class MotorSpeedController
{
private:
    // Target speed percent x 128.
    int16_t _targetSpeed = 0;
    // Current speed percent x 128.
    int16_t _currentSpeed = 0;
    // The amount to increase or decrease the current speed every 20th of a second.
    int16_t _accelerationStep = 0;

    bool _durationEnabled = false;
    uint32_t _durationTimeMs = 0;


public:

    void init()
    {
        _targetSpeed = 0;
        _currentSpeed = 0;
        _accelerationStep = 0;

        _durationEnabled = false;
        _durationTimeMs = 0;
    }

    void setDuration(int16_t durationMs)
    {
        if (durationMs < 0)
        {
            _durationEnabled = false;
        }
        else
        {
            _durationEnabled = true;
            _durationTimeMs = millis() + (uint32_t)durationMs;
        }
    }

    bool durationEnabled() const
    {
        return _durationEnabled;
    }

    bool durationExpired()
    {
        if (_durationEnabled && millis() >= _durationTimeMs)
        {
            stopAccelerating();
            _durationEnabled = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    // Set current speed without acelerating.
    void overrideAcceleration (int8_t speedPercent)
    {
        speedPercent = constrain(speedPercent, -100, 100);
        _currentSpeed = (int16_t)speedPercent * 128;
        _targetSpeed = _currentSpeed;
    }

    // Sets target speed to acelerate to.
    void setTargetSpeed(int8_t speedPercent)
    {
        speedPercent = constrain(speedPercent, -100, 100);
        _targetSpeed = (int16_t)speedPercent * 128;
    }

    /**
     * Sets the acceleration step (as speed percent x 128) to be applied every 20th of a second.
     * A value of 0 indicates no acceleration, i.e. instant speed change.
     */
    void setAcceleration(uint16_t accel)
    {
        _accelerationStep = constrain (accel, 0, 12800);
    }

    // Returns true if acceleration step is non-zero.
    bool accelerationEnabled() const
    {
        return _accelerationStep != 0;
    }

    // Returns true if target speed has not been reached.
    bool isAccelerating() const
    {
        return _targetSpeed != _currentSpeed;
    }

    // Sets target speed to current speed.
    void stopAccelerating()
    {
        _targetSpeed = _currentSpeed;
    }

    /**
     * Applies acceleration step to current speed in order to reach target speed.
     * Must be called every 20th of a second.
     */
    int8_t calculateNewSpeed()
    {
        if (_accelerationStep == 0)
        {
            _currentSpeed = _targetSpeed;
        }
        else if (_targetSpeed < _currentSpeed)
        {
            _currentSpeed = (_currentSpeed - _accelerationStep);

            if (_currentSpeed < _targetSpeed)
            {
                _currentSpeed = _targetSpeed;
            }
        }
        else if (_targetSpeed > _currentSpeed)
        {
            _currentSpeed = (_currentSpeed + _accelerationStep);

            if (_currentSpeed > _targetSpeed)
            {
                _currentSpeed = _targetSpeed;
            }
        }
        return (int8_t)(_currentSpeed >> 7);
    }
};

#endif