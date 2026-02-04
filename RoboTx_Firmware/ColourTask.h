#ifndef COLOUR_TASK_H
#define COLOUR_TASK_H

#include "Task.h"
#include "ColourMessage.h"
#include "Config.h"
#include "Settings.h"

#define COLOUR_TASK_INTERVAL_MS 102

class ColourMessage;

#if !defined(COLOUR_SENSING_ENABLED)

// On the Leonardo, the I2C pins clash with digital pins in use for other things (e.g. IR receiver).
class ColourTask : public Task
{
protected:
    void initialize(){}
public:
    ColourTask(Config *config){}
    void setColourMessage(ColourMessage *colourMessage){}
    void doEvents(){}
    void setRGB(){}
    uint16_t getR() const{ return 0;}
    uint16_t getG() const{ return 0;}
    uint16_t getB() const{ return 0;}
    uint16_t getClear() const{ return 0;}
    void enable(){}
    void enable(uint8_t integrationTimeIdx, uint8_t gain){}

    bool enabled() const
    {
        return false;
    }

    void disable(){}
};

#else

#include "TCS34725_Lite.h"

/**
 * Reads colour values reported by the colour sensor.
 * The inital state of the task is disabled and un-initialized.
 * This allows the I2C pins to be used by other devices as long as
 * the task remains in this state.
 */
class ColourTask : public Task
{
private:
    TCS34725_Lite tcs = TCS34725_Lite();

    uint16_t _c = 0;
    uint16_t _r = 0;
    uint16_t _g = 0;
    uint16_t _b = 0;

    bool _enabled = false;

    ColourMessage *_colourMessage;
    Config *_config;
    bool _initialized = false;

    uint16_t _colourTaskIntervalMs = COLOUR_TASK_INTERVAL_MS;

protected:
    /**
     * Initializes the sensor and indicate that the I2C bus is in use. 
     */
    void initialize()
    {
        if (!_initialized)
        {
            _config->setI2CInUse(true);
            tcs.begin();
            _initialized = true;
        }
    }

public:
    /**
     * Constructs instance with reference to Config and the message sender, which regulates the sending of
     * messages over Serial.
     */
    ColourTask(Config *config)
    {
        _config = config;
    }

    /**
     * Establish relationship with the ColourMessage that will serialize the analog readings.
     */
    void setColourMessage(ColourMessage *colourMessage)
    {
        _colourMessage = colourMessage;
    }

    /**
     * Cycles between task states, and queues response message.
     */
    void doEvents();

    /**
     * Reads colour values from sensor and stores locally.
     */
    void setRGB()
    {
        tcs.getRGBC(&_r, &_g, &_b, &_c);
    }

    /**
     * Get red component.
     */
    uint16_t getR() const
    {
        return _r;
    }

    /**
     * Get green component.
     */
    uint16_t getG() const
    {
        return _g;
    }

    /**
     * Get blue component.
     */
    uint16_t getB() const
    {
        return _b;
    }

    /**
     * Get clear component.
     */
    uint16_t getClear() const
    {
        return _c;
    }


    /**
     * Enables the task and starts collecting sensor values.
     * This method is not implemented for Leonardo.
     */
    void enable()
    {
        enable(2, 1);
    }

    /**
     * Enables the task and starts collecting sensor values using integration time and gain.
     * This method is not implemented for Leonardo.
     */
    void enable(uint8_t integrationTimeIdx, uint8_t gain)
    {
        if (!_initialized)
        {
            initialize();
        }
        else
        {
            tcs.enable();
        }
        if (!_enabled)
        {
            setTime();

            integrationTimeIdx = constrain(integrationTimeIdx, 0, 5);
            gain = constrain(gain, 0, 3);

            uint8_t integrationTime;

            switch (integrationTimeIdx)
            {
            case 0:
            {
                integrationTime = TCS_INTEGRATIONTIME_2_4MS;
                _colourTaskIntervalMs = 100;
                break;
            }
            case 1:
            {
                integrationTime = TCS_INTEGRATIONTIME_24MS;
                _colourTaskIntervalMs = 100;
                break;
            }
            case 3:
            {
                integrationTime = TCS_INTEGRATIONTIME_101MS;
                _colourTaskIntervalMs = 102;
                break;
            }
            case 4:
            {
                integrationTime = TCS_INTEGRATIONTIME_154MS;
                _colourTaskIntervalMs = 155;
                break;
            }
            case 5:
            {
                integrationTime = TCS_INTEGRATIONTIME_700MS;
                _colourTaskIntervalMs = 701;
                break;
            }
            default: // case 2:
            {
                integrationTime = TCS_INTEGRATIONTIME_50MS;
                _colourTaskIntervalMs = 100;
                break;
            }
            }
            tcs.setIntegrationtime(integrationTime);
            tcs.setGain(gain);
        }
        _enabled = true;

    }

    // Returns true if the task is enabled, false otherwise.
    bool enabled() const
    {
        return _enabled;
    }

    /**
     * Disables the task.
     * This method is not implemented for Leonardo.
     */
    void disable()
    {

        if (_enabled)
        {
            tcs.disable();
        }
        _enabled = false;

    }
};
#endif
#endif