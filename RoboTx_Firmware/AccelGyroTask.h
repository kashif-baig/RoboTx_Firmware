#ifndef ACCEL_GYRO_TASK_H
#define ACCEL_GYRO_TASK_H

#include "Task.h"
#include "AccelGyroMessage.h"
#include "Config.h"
#include "Settings.h"

#define ACCEL_GYRO_TASK_INTERVAL_MS 15

#if !defined(MPU6050_SENSOR_AVAILABLE)

// On the Leonardo, the I2C pins clash with digital pins in use for other things (e.g. IR receiver).
class AccelGyroTask : public Task
{
protected:
    void initialize() {}

public:
    AccelGyroTask(Config *config) {}
    void setAccelGyroMessage(AccelGyroMessage *accelGyroMessage) {}
    void doEvents() {}

    void enable(uint8_t accelScale = 0, uint8_t gyroScale = 0) {}

    bool enabled() const
    {
        return false;
    }

    void disable() {}
};

#else

#include "MPU6050_Lite.h"

#ifndef MPU_ACCEL_SCALE
#define MPU_ACCEL_SCALE 0
#endif

#ifndef MPU_GYRO_SCALE
#define MPU_GYRO_SCALE 0
#endif

/**
 * AccelGyroTask FSM states.
 */
enum AccelGyroTaskState : uint8_t
{
    AccelGyroTaskState_Delay,
    AccelGyroTaskState_SendAccel,
    AccelGyroTaskState_SendGyro
};

/**
 * Reads acceleration and gyro values reported by the MPU6050 sensor.
 * The inital state of the task is disabled and un-initialized.
 * This allows the I2C pins to be used by other devices as long as
 * the task remains in this state.
 */
class AccelGyroTask : public Task
{
private:
    I2C i2c = I2C(&Wire, MPU_DEFAULT_ADDRESS);
    MPU6050 _mpu = MPU6050(&i2c);

    bool _enabled = false;

    AccelGyroMessage *_accelGyroMessage;
    Config *_config;
    bool _initialized = false;

    AccelGyroTaskState _accelGyroTaskState;

protected:
    /**
     * Initializes the sensor and indicate that the I2C bus is in use.
     */
    void initialize()
    {
        if (!_initialized)
        {
            if (_mpu.begin())
            {
                _mpu.setAccelScale(MPU_ACCEL_SCALE);
                _mpu.setGyroScale(MPU_GYRO_SCALE);

                _config->setI2CInUse(true);
                _initialized = true;
            }
        }
    }

public:
    /**
     * Constructs instance with reference to Config and the message sender, which regulates the sending of
     * messages over Serial.
     */
    AccelGyroTask(Config *config)
    {
        _config = config;
    }

    /**
     * Establish relationship with the ColourMessage that will serialize the analog readings.
     */
    void setAccelGyroMessage(AccelGyroMessage *accelGyroMessage)
    {
        _accelGyroMessage = accelGyroMessage;
    }

    /**
     * Cycles between task states, and queues response message.
     */
    void doEvents();

    /// @brief Enables the MPU sensor with given default values.
    void enable()
    {
        if (!_initialized)
        {
            initialize();
        }
        else
        {
            _mpu.enable();
        }

        if (!_enabled && _initialized)
        {
            setTime();
            _accelGyroTaskState = AccelGyroTaskState_Delay;
            _enabled = true;
        }
    }

    // Returns true if the task is enabled, false otherwise.
    bool enabled() const
    {
        return _enabled;
    }

    /**
     * Disables the task.
     */
    void disable()
    {

        if (_enabled)
        {
            _mpu.disable();
        }
        _enabled = false;
    }
};
#endif
#endif