#ifndef SWITCH_MANAGER_H
#define SWITCH_MANAGER_H

#include "Settings.h"

#define SWITCH_COUNT    4

/**
 * Manages digit outputs to switch devices on and off. The initial state of the Arduino
 * pins used for digital output depends on their switch on value. These pins may
 * be used by other devices if their inital state is the same. 
 */
class SwitchManager
{
public:
    SwitchManager(uint8_t switch1Pin, uint8_t switch2Pin, uint8_t switch3Pin, uint8_t switch4Pin)
    {
        _switchPin[0] = SWITCH1_ENABLED ? switch1Pin : -1;
        _switchPin[1] = SWITCH2_ENABLED ? switch2Pin : -1;
        _switchPin[2] = SWITCH3_ENABLED ? switch3Pin : -1;
        _switchPin[3] = SWITCH4_ENABLED ? switch4Pin : -1;
    }

    // Initialize output pins.
    void initialize()
    {
        for (uint8_t i=0; i < SWITCH_COUNT; i++)
        {
            int8_t switchPin = _switchPin[i];

            if (switchPin > -1)
            {
                pinMode(switchPin, OUTPUT);
                digitalWrite(switchPin, !_switchOnVal[0]);
            }
        }
    }

    // Sets switch output on status.
    void setSwitchOn(uint8_t switchNum, bool on)
    {
        switchNum = constrain(switchNum-1, 0, SWITCH_COUNT-1);
        int8_t switchPin = _switchPin[switchNum];

        if (switchPin > -1)
        {
            digitalWrite(switchPin, on ? _switchOnVal[switchNum] : !_switchOnVal[switchNum]);
        }
    }

    // Sets all switch outputs to off.
    void setSwitchesOff()
    {
        for (uint8_t i=0; i <= SWITCH_COUNT; i++)
        {
            setSwitchOn(i, false);
        }
    }

private:
    int8_t _switchPin[SWITCH_COUNT] = {-1,-1,-1,-1};
    uint8_t _switchOnVal[SWITCH_COUNT] = {SWITCH1_ON_VALUE, SWITCH2_ON_VALUE, SWITCH3_ON_VALUE, SWITCH4_ON_VALUE};
};

#endif
