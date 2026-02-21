#ifndef DHT_TASK_H
#define DHT_TASK_H

#include "Task.h"
#include "DHTMessage.h"
#include "Config.h"
#include "Settings.h"

//#include "Debug.h"

#define DHT_TASK_INTERVAL_MS 987

class DHTMessage;

#if !defined(DHT20_SENSOR_AVAILABLE)

// On the Leonardo, the I2C pins clash with digital pins in use for other things (e.g. IR receiver).
class DHTTask : public Task
{
protected:
    void initialize(){}
public:
    DHTTask(Config *config){}
    void setDHTMessage(DHTMessage *dhtMessage){}
    void doEvents(){}
    
    void enable(){}

    bool enabled() const
    {
        return false;
    }

    void disable(){}
};

#else

#include "DHT20_Lite.h"

#ifndef DHT20_ADDRESS
  #define DHT20_ADDRESS 0x38
#endif

/**
 * DHTTaskState FSM states.
 */
enum DHTTaskState : uint8_t
{
    DHTTaskState_ReadTemp,
    DHTTaskState_ReadHumidity,
    DHTTaskState_Send,
    DHTTaskState_Delay
};

/**
 * Reads temperature and humidity values reported by the DHT sensor.
 * The inital state of the task is disabled and un-initialized.
 * This allows the I2C pins to be used by other devices as long as
 * the task remains in this state.
 */
class DHTTask : public Task
{
private:
    I2C i2c = I2C(&Wire, DHT20_ADDRESS);
    DHT20_Lite dht20 = DHT20_Lite(&i2c);
    
    //DHT20_Lite dht20 = DHT20_Lite(&Wire, DHT20_ADDRESS);

    DHTTaskState _dhtTaskState;
    DHTMessage *_dhtMessage;
    Config *_config;
    bool _enabled = false;
    bool _initialized = false;

    //const uint16_t _dhtTaskIntervalMs = DHT_TASK_INTERVAL_MS;

protected:
    bool initialize()
    {
        if (!_initialized)
        {
            //Debug.println("DHT init start");

            if (dht20.begin())
            {
                _config->setI2CInUse(true);
                _initialized = true;

                //Debug.println("DHT init OK");
            }
            else
            {
                return false;
            }
        }
        return true;
    }

public:
    DHTTask(Config *config)
    {
        _config = config;
    }
    
    void setDHTMessage(DHTMessage *dhtMessage)
    {
        _dhtMessage = dhtMessage;
    }
    
    void doEvents();

    // Enables the DHT sensor and begins the reporting of temperature and humidity values.
    void enable()
    {
        if (!_initialized)
        {
            initialize();
        }

        if (!_enabled && _initialized)
        {
            setTime();
            _dhtTaskState = DHTTaskState_ReadTemp;
            _enabled = true;
        }
    }


    // Returns true if the task is enabled, false otherwise.
    bool enabled() const
    {
        return _enabled;
    }

    /**
     * Disables the task, which prevents the reporting of temperature and humidity.
     */
    void disable()
    {
        if (_enabled)
        {

        }
        _enabled = false;
    }
};

#endif
#endif