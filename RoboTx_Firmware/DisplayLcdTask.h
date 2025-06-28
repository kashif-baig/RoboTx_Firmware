#ifndef DISPLAYLCD_TASK_H
#define DISPLAYLCD_TASK_H

#include "Task.h"
#include "Config.h"
#include <LiquidCrystal_I2C.h>
#include "Settings.h"

/**
 * Writes text to the I2C LCD display.
 * The inital state of the task is disabled and un-initialized.
 * This allows the I2C pins to be used by other devices as long as
 * the task remains in this state.
 */
class DisplayLcdTask : public Task
{
private:

    Config *_config;
    LiquidCrystal_I2C *_lcd;
    bool        _initialized = false;
    uint32_t    _lightOnTime;
    bool        _lightIsOn = false;
    bool        _enabled = false;

protected:
    /**
     * Initializes the display and indicate that the I2C bus is in use. 
     */
    void initialize()
    {
        if (!_initialized)
        {
            _config->setI2CInUse(true);
            _lcd->init();
            _initialized = true;
            _lightOnTime = millis();
        }
        lightOff();
    }
public:

    DisplayLcdTask(Config *config, LiquidCrystal_I2C *lcd)
    {
        _config = config;
        _lcd = lcd;
    }

    /**
     * Prints text at specified position on LCD display.
     */
    void printAt(uint8_t col, uint8_t row, const char* text)
    {
        if (_enabled)
        {
            _lcd->setCursor(col, row);
            _lcd->print(text);

            lightOn();
        }
    }

    // Enables the task, initialising the I2C bus if not already done so.
    void enable()
    {
#if defined(DISPLAY_LCD_ENABLED)
        if (!_enabled)
        {
            initialize();
            _lcd->clear();
            _enabled = true;
        }
#endif
    }

    // Clears the LCD display.
    void clear()
    {
        if (_enabled)
        {
            _lcd->clear();
            _lcd->setCursor(0, 0);
            //lightOn();
        }
    }

    // Returns true if the task is enabled.
    bool enabled() const
    {
        return _enabled;
    }

    // Disables the task.
    void disable()
    {
        if (_enabled)
        {
            lightOff();
            _enabled = false;
        }
    }

    // Manages the backlight timeout.
    void doEvents()
    {
    #if defined(DISPLAY_LCD_ENABLED)
        if (_lightIsOn && DISPLAY_LCD_LIGHT_DURATION!=0 && millis() - _lightOnTime > (DISPLAY_LCD_LIGHT_DURATION * 1000))
        {
            lightOff();
        }
    #endif
    }

    // Turns on the backlight for a defined duration.
    void lightOn()
    {
        if (_enabled)
        {
            _lcd->backlight();
            _lightIsOn = true;
            _lightOnTime = millis();
        }
    }

    // Turns off the backlight.
    void lightOff()
    {
        if (_enabled)
        {
            _lightIsOn = false;
            _lcd->noBacklight();
        }
    }
};

#endif