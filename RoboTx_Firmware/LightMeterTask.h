#ifndef LIGHTMETER_TASK_H
#define LIGHTMETER_TASK_H

#include "Task.h"
#include "LightMeterMessage.h"
#include "Config.h"
#include "Settings.h"

// #include "Debug.h"

#ifndef BH1750_ADDRESS
    #define BH1750_ADDRESS 0x5c
#endif

#define LIGHTMETER_TASK_INTERVAL_MS 129

class LightMeterMessage;

#if !defined(LIGHTMETER_SENSOR_AVAILABLE)

// On the Leonardo, the I2C pins clash with digital pins in use for other things (e.g. IR receiver).
class LightMeterTask : public Task
{
protected:
    void initialize() {}

public:
    LightMeterTask(Config *config) {}
    void setLightMeterMessage(LightMeterMessage *lightMeterMessage) {}
    void doEvents() {}
    uint16_t getLux() const { return 0; }
    void enable() {}

    bool enabled() const
    {
        return false;
    }

    void disable() {}
};

#else

#include "BH1750_Lite.h"

/**
 * Reads LUX values reported by the light meter sensor.
 * The inital state of the task is disabled and un-initialized.
 * This allows the I2C pins to be used by other devices as long as
 * the task remains in this state.
 */
class LightMeterTask : public Task
{
private:
    I2C i2c = I2C(&Wire, BH1750_ADDRESS);

    BH1750_Lite _lightMeter = BH1750_Lite(&i2c);

    uint16_t _lux = 0;
    bool _enabled = false;
    LightMeterMessage *_lightMeterMessage;
    Config *_config;
    bool _initialized = false;

protected:
    bool initialize()
    {
        if (!_initialized)
        {
            // Debug.println("Light meter begin");
            i2c.begin();

            if (_lightMeter.begin())
            {
                // Debug.println("Light meter has begun.");
                _config->setI2CInUse(true);
                _initialized = true;
            }
            else
            {
                return false;
            }
        }
        return _initialized;
    }

public:
    LightMeterTask(Config *config)
    {
        _config = config;
    }

    void setLightMeterMessage(LightMeterMessage *lightMeterMessage)
    {
        _lightMeterMessage = lightMeterMessage;
    }

    void doEvents();

    void enable()
    {
        if (!_initialized)
        {
            initialize();
        }
        if (!_enabled && _initialized)
        {
            setTime();
            _lightMeter.configure(BH1750_Lite::CONTINUOUS_HIGH_RES_MODE);
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
            _lightMeter.configure(BH1750_Lite::UNCONFIGURED);
        }
        _enabled = false;
    }
};

#endif
#endif