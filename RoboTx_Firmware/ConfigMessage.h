#ifndef CONFIG_MESSAGE_H
#define CONFIG_MESSAGE_H

#include <MessagingLib.h>
#include "Config.h"

enum ConfigCmd : uint8_t
{
    ConfigCmdNone,
    ConfigCmdInpEvt,
    ConfigCmdInpAnlg,
    ConfigCmdInpInv,
};

const char *ConfigMessageType = "CFG";

/**
 * Receives firmware configuration related messages.
 */
class ConfigMessage : public IDeserializableMessage
{
public:

    ConfigMessage(Config *config)
    {
        _config = config;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return ConfigMessageType;
    }

    void onBeginDeserialize()
    {
        _cmd = ConfigCmdNone;
        _param1Text[0] = 0;
        _param2Text[0] = 0;
    }

    /**
     * Deserializes properties (fields) in the order they were serialized by the client.
     */
    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
    {
        switch (propertyIndex)
        {
            case 0:
            {
                if (strcasecmp_P(propertyValue, PSTR("InpInv"))==0)
                {
                    _cmd = ConfigCmdInpInv;
                }
                else if (strcasecmp_P(propertyValue, PSTR("InpEvt"))==0)
                {
                    _cmd = ConfigCmdInpEvt;
                }
                else if (strcasecmp_P(propertyValue, PSTR("InpAnlg"))==0)
                {
                    _cmd = ConfigCmdInpAnlg;
                }
                break;
            }
            case 1:
            {
                strncat(_param1Text, propertyValue, sizeof(_param1Text) - 1);
                break;
            }
            case 2:
            {
                strncat(_param2Text, propertyValue, sizeof(_param2Text) - 1);
                break;
            }
        }
    }

    /**
     * Processes the deserialized request.
     */
    void onEndDeserialize(bool messageComplete)
    {
        if (messageComplete)
        {
            if (_cmd == ConfigCmdInpEvt)
            {
                bool enabled = _param1Text[0] == 'T';

                uint8_t param2Len = strlen(_param2Text);

                for (uint8_t i = 0; i < param2Len; i++)
                {
                    if (_param2Text[i] == '1')
                    {
                        // Bit positions map to digital pins.
                        uint8_t analogPinIdx = param2Len-1 - i;
                        _config->enableDigitalInput(analogPinIdx, enabled);

                        if (!enabled)
                        {
                            // Un-invert a disabled input.
                            _config->invertDigitalInput(analogPinIdx, false);
                        }
                    }
                }
            }
            else if (_cmd == ConfigCmdInpAnlg)
            {
                bool enabled = _param1Text[0] == 'T';

                uint8_t param2Len = strlen(_param2Text);

                for (uint8_t i = 0; i < param2Len; i++)
                {
                    if (_param2Text[i] == '1')
                    {
                        // Assuming bit positions map to pins A0 to A7.
                        uint8_t analogPinIdx = param2Len-1 - i;
                        _config->enableAnalogInput(analogPinIdx, enabled);
                    }
                }
            }
            else if (_cmd == ConfigCmdInpInv)
            {
                bool inverted = _param1Text[0] == 'T';
                uint8_t param2Len = strlen(_param2Text);

                for (uint8_t i = 0; i < param2Len; i++)
                {
                    if (_param2Text[i] == '1')
                    {
                        // Assuming bit positions map to digital pins A0 to A4.
                        uint8_t analogPinIdx = param2Len-1 - i;
                        _config->invertDigitalInput(analogPinIdx, inverted);
                    }
                }
            }
        }
    }

private:
    Config *_config;
    ConfigCmd _cmd;
    char _param1Text[5];
    char _param2Text[9];
};

#endif
