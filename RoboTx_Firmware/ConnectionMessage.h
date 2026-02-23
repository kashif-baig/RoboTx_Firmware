#ifndef CONNECTION_MESSAGE_H
#define CONNECTION_MESSAGE_H

#include <MessagingLib.h>
#include "TimerIO.h"
#include "ServoManager.h"
#include "Config.h"
#include "AnalogTask.h"
#include "Connection.h"
#include "PulseCounterTask.h"
#include "Display7SegTask.h"
#include "ColourTask.h"
#include "LightMeterTask.h"
#include "DHTTask.h"
#include "AccelGyroTask.h"
#include "SwitchManager.h"
#include "DisplayLcdTask.h"
#include "Settings.h"

#define CR_CMD_OPEN     'O'
#define CR_CMD_CLOSE    'C'
#define CR_CMD_KEEP_ALIVE 'A'

const char *ConnectionMessageType = "C";

/**
 * Receives serial connection related messages, such as client signalling closing of connection.
 * On such an event, peripherals may be powered down, and tasks that send continuous stream
 * of data to the serial client are disabled.
 */
class ConnectionMessage : public IDeserializableMessage
{
public:

    ConnectionMessage (Config *config, Connection *connection, Display7SegTask *display7SegTask, ServoManager *servoManager,
                        TimerIO *timerIO, AnalogTask *analogTask, PulseCounterTask *pulseCounterTask, ColourTask *colourTask,
                        SwitchManager *switchManager, DisplayLcdTask *displayLcdTask,
                        LightMeterTask *lightMeterTask, DHTTask *dhtTask, AccelGyroTask *accelGyroTask)
    {
        _config = config;
        _connection = connection;
        _display7SegTask = display7SegTask;
        _servoManager = servoManager;
        _timerIO = timerIO;
        _analogTask = analogTask;
        _pulseCounterTask = pulseCounterTask;
        _colourTask = colourTask;
        _switchManager = switchManager;
        _displayLcdTask = displayLcdTask;
        _lightMeterTask = lightMeterTask;
        _dhtTask = dhtTask;
        _accelGyroTask = accelGyroTask;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return ConnectionMessageType;
    }

    /**
     * Deserializes properties (fields) in the order they were serialized by the client.
     */
    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
    {
        switch (propertyIndex)
        {
        case 0:
            _cmd = *propertyValue;
            break;
        }
    }

    /**
     * Processes the deserialized request.
     */
    void onEndDeserialize(bool messageComplete)
    {
        if (messageComplete)
        {
            if (_cmd == CR_CMD_OPEN || _cmd == CR_CMD_KEEP_ALIVE && !_connection->isOpen())
            {
                // MsgSerial.write(MSG_START);
                // writeToStream(&MsgSerial, PSTR("RTx"));
                // MsgSerial.write(MSG_PROPERTY);
                // writeToStream(&MsgSerial, PSTR(ROBOT_ID));
                // MsgSerial.write(MSG_END);
                // _timerIO->beep();
            }
            if (_cmd == CR_CMD_OPEN || _cmd == CR_CMD_KEEP_ALIVE)
            {
                if (_connection->isOpen())
                {
                    if (_cmd == CR_CMD_KEEP_ALIVE)
                    {
                        _connection->keepAlive();
                        return;
                    }
                }
                else
                {
                    MsgSerial.write(MSG_START);
                    writeToStream(&MsgSerial, PSTR("RTx"));
                    MsgSerial.write(MSG_PROPERTY);
                    writeToStream(&MsgSerial, PSTR(ROBOT_ID));
                    MsgSerial.write(MSG_END);
                    //_timerIO->beep();
                }
                _connection->setOpen(true);
                _analogTask->enable();

#if defined(DIGITAL_INPUTS_ENABLED)
                uint8_t pinIdx3[] = {DIGITAL_INPUTS_ENABLED};

                for (uint8_t i = 0; i < sizeof(pinIdx3); i++)
                {
                    _config->enableDigitalInput(pinIdx3[i], true);
                }
#endif
            }
            else if (_cmd == CR_CMD_CLOSE)
            {
                _connection->setOpen(false);
                _analogTask->disable();
                _displayLcdTask->disable();
                _timerIO->setInputsEnabled(false);
            }

            //_timerIO->beep(0);
            
            _display7SegTask->setEnabled(false);
            if (_config->motorTaskAvailable())
            {
                _config->getMotorTask()->disable();
            }
            _servoManager->disableServos();
            _pulseCounterTask->disable();
            _colourTask->disable();
            _lightMeterTask->disable();
            _dhtTask->disable();
            _accelGyroTask->disable();
            
#if !defined(DIGITAL_INPUTS_ENABLED)
            //_timerIO->setInputsEnabled(false);
#endif
#if !defined(DIGITAL_INPUTS_INVERTED)
            _timerIO->setInputsInverted(false);
#endif
            _switchManager->setSwitchesOff();
        }
    }

private:
    char _cmd;
    Config *_config;
    Connection *_connection;
    Display7SegTask *_display7SegTask;
    ServoManager *_servoManager;
    TimerIO *_timerIO;
    AnalogTask *_analogTask;
    PulseCounterTask *_pulseCounterTask;
    ColourTask *_colourTask;
    SwitchManager *_switchManager;
    DisplayLcdTask *_displayLcdTask;
    LightMeterTask *_lightMeterTask;
    DHTTask *_dhtTask;
    AccelGyroTask *_accelGyroTask;
};
#endif
