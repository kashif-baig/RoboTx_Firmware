#ifndef MY_DESERIALIZABLE_MESSAGE_FACTORY_H
#define MY_DESERIALIZABLE_MESSAGE_FACTORY_H

#include <MessagingLib.h>
#include "TimerIO.h"
#include "Display7SegMessage.h"
#include "BeeperMessage.h"
#include "AnalogMessage.h"
#include "MotorMessage.h"
#include "ServoMessage.h"
#include "SonarMessage.h"
#include "ConnectionMessage.h"
#include "ConfigMessage.h"
#include "PulseCounterMessage.h"
#include "ColourMessage.h"
#include "SwitchMessage.h"
#include "DisplayLcdMessage.h"
#include <new>

// Determine amount of memory to allocate for messages.
static const size_t LARGEST_MESSAGE_SIZE =
    STATIC_MAX(sizeof(Display7SegMessage),
    STATIC_MAX(sizeof(BeeperMessage),
    STATIC_MAX(sizeof(MotorMessage),
    STATIC_MAX(sizeof(ServoMessage),
    STATIC_MAX(sizeof(ConnectionMessage),
    STATIC_MAX(sizeof(ConfigMessage),
    STATIC_MAX(sizeof(SwitchMessage),
    STATIC_MAX(sizeof(DisplayLcdMessage),
    0))))))));

// Pre-allocated memory for messages. Align to 4 byte boundary.

union
{
    char Memory[LARGEST_MESSAGE_SIZE];
    uint32_t dummy;
} MessageUnion;

static char* Message = MessageUnion.Memory;


/**
 * Instantiates Deserializable message classes based on the incoming message type.
 */
class MyDeserializableMessageFactory : public IDeserializableMessageFactory
{
public:
    MyDeserializableMessageFactory(
            Config *config, Display7SegTask *display7SegTask, TimerIO *timerIO, Connection *connection,
            AnalogMessage *analogMessage, AnalogTask *analogTask, SonarMessage *sonarMessage,
            ServoManager *servoManager, PulseCounterMessage *pulseCounterMessage, PulseCounterTask *pulseCounterTask,
            ColourMessage *colourMessage, ColourTask *colourTask, SwitchManager *switchManager,
            DisplayLcdTask *displayLcdTask)
    {
        _config = config;
        _timerIO = timerIO;
        _display7SegTask = display7SegTask;
        _connection = connection;
        _analogMessage = analogMessage;
        _analogTask = analogTask;
        _sonarMessage = sonarMessage;
        _servoManager = servoManager;
        _pulseCounterMessage = pulseCounterMessage;
        _pulseCounterTask = pulseCounterTask;
        _colourMessage = colourMessage;
        _colourTask = colourTask;
        _switchManager = switchManager;
        _displayLcdTask = displayLcdTask;
    }

protected:
    IDeserializableMessage *getMessageDeserializer(const char *messageType)
    {
        IMotorTask *motorTask = _config->getMotorTask();

        if (strcmp(messageType, MotorMessageType) == 0)
        {
            if (_config->motorTaskAvailable() && !motorTask->isEnabled())
            {
                motorTask->enable();
            }
            return new (Message) MotorMessage(_config);
        }
        if (strcmp(messageType, ServoMessageType) == 0)
        {
            return new (Message) ServoMessage(_servoManager, _config);
        }
        if (strcmp(messageType, SonarMessageType) == 0)
        {
            return _sonarMessage;
        }
        if (strcmp(messageType, SwitchMessageType) == 0)
        {
            return new (Message) SwitchMessage(_switchManager);
        }
        if (strcmp(messageType, DisplayLcdMessageType) == 0)
        {
            if (!_displayLcdTask->enabled())
            {
                _displayLcdTask->enable();
            }
            return new (Message) DisplayLcdMessage(_displayLcdTask);
        }
        if (strcmp(messageType, Display7SegMessageType) == 0)
        {
            if (!_display7SegTask->isEnabled())
            {
                _display7SegTask->setEnabled(true);
            }
            return new (Message) Display7SegMessage(_display7SegTask);
        }
        if (strcmp(messageType, BeeperMessageType) == 0)
        {
            return new (Message) BeeperMessage(_timerIO);
        }
        if (strcmp(messageType, AnalogMessageType) == 0)
        {
            return _analogMessage;
        }
        if (strcmp(messageType, PulseCounterMessageType) == 0)
        {
            return _pulseCounterMessage;
        }
        if (strcmp(messageType, ColourMessageType) == 0)
        {
            return _colourMessage;
        }
        if (strcmp(messageType, ConnectionMessageType) == 0)
        {
            return new (Message) ConnectionMessage(_config, _connection, _display7SegTask, _servoManager,
                                                    _timerIO, _analogTask, _pulseCounterTask, _colourTask,
                                                    _switchManager, _displayLcdTask);
        }
        if (strcmp(messageType, ConfigMessageType) == 0)
        {
            return new (Message) ConfigMessage(_config);
        }
        return NULL;
    }

private:
    Config *_config;
    TimerIO *_timerIO;
    Display7SegTask *_display7SegTask;
    Connection *_connection;
    AnalogMessage *_analogMessage;
    AnalogTask *_analogTask;
    SonarMessage *_sonarMessage;
    ServoManager *_servoManager;
    PulseCounterMessage *_pulseCounterMessage;
    PulseCounterTask *_pulseCounterTask;
    ColourMessage *_colourMessage;
    ColourTask *_colourTask;
    SwitchManager *_switchManager;
    DisplayLcdTask *_displayLcdTask;

    Print *_outStream;
    SerializableMessageSender *_serMsgSender;
};

#endif