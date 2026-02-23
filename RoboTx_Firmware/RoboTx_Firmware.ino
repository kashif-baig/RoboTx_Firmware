/**
 * Robo-Tx firmware by Kashif Baig. For use by Robo-Tx API installed on a computer.
 * 
 * The following Arduino libraries will need to be installed:
 * 
 * IRremote
 * LiquidCrystal I2C
 * Servo
 * LiquidTWI2 (https://github.com/lincomatic/LiquidTWI2)
 * MessagingLib
 * StringLib
 * 
 * See file Settings.h for configuring pin assignments and Bluetooth.
 * 
 * https://www.cohesivecomputing.co.uk/robo-tx/
 * 
 */

#include "TimerIO.h"
#include <MessagingLib.h>
#include "MyDeserializableMessageFactory.h"
#include "Settings.h"
#include "ButtonMessage.h"
#include "AnalogTask.h"
#include "AnalogMessage.h"
#include "SonarTask.h"
#include "SonarMessage.h"
#include "ServoManager.h"
#include "IRMessage.h"
#include "Connection.h"
#include "Config.h"
#include "PulseCounterMessage.h"
#include "PulseCounterTask.h"
#include "Display7SegTask.h"
#include "SwitchManager.h"
#include "ColourTask.h"
#include "LightMeterTask.h"
#include "DHTTask.h"
#include "AccelGyroTask.h"
#include "DisplayLcdTask.h"
#include "I2CRequestMessage.h"

#if defined(IR_REMOTE_ENABLED)
    #define DECODE_NEC
    #define DECODE_LEGO_PF
    #include <IRremote.hpp>
#endif

AnalogTask analogTask = AnalogTask();
AnalogMessage analogMessage = AnalogMessage(&analogTask);

SonarTask sonarTask = SonarTask(SONAR_TRIG_PIN, SONAR_ECHO_PIN);
SonarMessage sonarMessage = SonarMessage(&sonarTask);

Display7SegTask display7SegTask = Display7SegTask(LED_7SEG_LATCH_PIN, LED_7SEG_CLK_PIN, LED_7SEG_DATA_PIN);

ServoManager servoManager = ServoManager(SERVO1_PIN, SERVO2_PIN, SERVO3_PIN, SERVO4_PIN, SERVO5_PIN, SERVO6_PIN);

IrMessage irMessage;
Connection connection;

PulseCounterTask pulseCounterTask = PulseCounterTask();
PulseCounterMessage pulseCounterMessage = PulseCounterMessage(&TmrIO, &pulseCounterTask, &analogTask);

Config config = Config(&display7SegTask, &TmrIO, &pulseCounterTask, &analogTask);

ColourTask colourTask = ColourTask(&config);
ColourMessage colourMessage = ColourMessage(&colourTask);

LightMeterTask lightMeterTask = LightMeterTask(&config);
LightMeterMessage lightMeterMessage = LightMeterMessage();

DHTTask dhtTask = DHTTask(&config);
DHTMessage dhtMessage = DHTMessage();

AccelGyroTask accelGyroTask = AccelGyroTask(&config);
AccelGyroMessage accelGyroMessage = AccelGyroMessage();

I2CRequestMessage i2cMessage = I2CRequestMessage(&colourTask, &lightMeterTask, &dhtTask, &accelGyroTask);

SwitchManager switchManager = SwitchManager(SWITCH1_PIN, SWITCH2_PIN, SWITCH3_PIN, SWITCH4_PIN);

#if DISPLAY_LCD_I2C_MCP23008 == true
    LiquidTWI2 lcd(DISPLAY_LCD_I2C_ADDR);
#else
    LiquidCrystal_I2C lcd(DISPLAY_LCD_I2C_ADDR, DISPLAY_LCD_COLS, DISPLAY_LCD_ROWS);
#endif

DisplayLcdTask displayLcdTask(&config, &lcd);

ConnectionMessage connMessage = ConnectionMessage(&config, &connection, &display7SegTask, &servoManager,
                                                    &TmrIO, &analogTask, &pulseCounterTask, &colourTask,
                                                    &switchManager, &displayLcdTask, &lightMeterTask,
                                                    &dhtTask, &accelGyroTask);

MyDeserializableMessageFactory factory = MyDeserializableMessageFactory(&connMessage,
                                        &config, &display7SegTask, &TmrIO, &analogMessage,
                                        &sonarMessage, &servoManager, &pulseCounterMessage,
                                        &i2cMessage, &switchManager, &displayLcdTask);

// Buffer must be large enough to hold the largest property value of a message.
StringBuilder<17> msgBuffer;
MessageListener msgListener(&MsgSerial, &msgBuffer, &factory);

ButtonMessage buttonMessage;

void userISR();
void onSettingI2CInUse();
void beforeSonarPing();
void afterSonarPing();

void setup()
{
#if defined(__AVR_ATmega32U4__) // Leonardo
    // for bluetooth modules not using 5v TTL.
    pinMode(0, INPUT_PULLUP);  // Initialize serial RX pin.
#endif

#if DEFAULT_MOTOR_DRIVER == MOTOR_DRIVER_DIRPWM
    config.useMotorDriverDirPwm(MOTOR_DIRPWM_EN_DEFAULT);
#elif DEFAULT_MOTOR_DRIVER == MOTOR_DRIVER_ININ
    config.useMotorDriverInIn();
#else
    config.useMotorDriverHW56();
#endif

    config.onSettingI2CInUse = onSettingI2CInUse;

    display7SegTask.setSegmentRTL(LED_7SEG_RTL);
    display7SegTask.setEnabled(false);

    MsgSerial.begin(SERIAL_BAUD);
    
    // Set user interrupt service routine that will run 1000 times per second.
    TmrIO.setUserInterrupt(userISR);
    bool inputsInverted = false;
    bool beeperInverted = TRIGGER_ON_VALUE == LOW;
    TmrIO.initialize(beeperInverted, inputsInverted);

    //TmrIO.beep();

    analogTask.initialize();
    analogTask.disable();
    analogTask.setSampleRate(ANALOG_SAMPLE_FREQ_HZ);
    analogTask.setAnalogMessage(&analogMessage);

    sonarTask.initialize();
    sonarTask.setSonarMessage(&sonarMessage);
    sonarTask.onBeforePing = beforeSonarPing;
    sonarTask.onAfterPing = afterSonarPing;

    pulseCounterTask.disable();
    pulseCounterTask.setPulseCounterMessage(&pulseCounterMessage);

    colourTask.disable();
    colourTask.setColourMessage(&colourMessage);

    lightMeterTask.disable();
    lightMeterTask.setLightMeterMessage(&lightMeterMessage);

    dhtTask.disable();
    dhtTask.setDHTMessage(&dhtMessage);

    accelGyroTask.disable();
    accelGyroTask.setAccelGyroMessage(&accelGyroMessage);

    switchManager.initialize();

    displayLcdTask.disable();

#if defined(IR_REMOTE_ENABLED)
    IrReceiver.begin(2, false);
#endif

#if defined(ANALOG_ENABLED_A)
    {
        uint8_t analogPinIdx1[] = {ANALOG_ENABLED_A};

        for (uint8_t i = 0; i < sizeof(analogPinIdx1); i++)
        {
            config.enableAnalogInput(analogPinIdx1[i], true);
        }
    }
#endif

#if defined(DIGITAL_INPUTS_INVERTED)
    {
        uint8_t pinIdx2[] = {DIGITAL_INPUTS_INVERTED};

        for (uint8_t i = 0; i < sizeof(pinIdx2); i++)
        {
            config.invertDigitalInput(pinIdx2[i], true);
        }
    }
#endif

    if (MOTOR_TASK_DISABLE_ON_BOOT && config.motorTaskAvailable())
    {
        config.getMotorTask()->disable();
    }
}

void loop()
{
    // Process incoming request messages over serial.
    msgListener.processMessageStream();

    // Check for input button states and send over serial.
    const uint8_t btn = TmrIO.getButton();
    if (btn)
    {
        if (connection.isOpen())
        {
            buttonMessage.setButtonValue(btn);
            buttonMessage.serialize(&MsgSerial);
        }
    }

    analogTask.doEvents();
    connection.doEvents();

    if (config.motorTaskAvailable())
    {
        IMotorTask *motorTask = config.getMotorTask();
        motorTask->informConnectionState(connection.isAlive());
        motorTask->doEvents();
    }
    pulseCounterTask.doEvents();
    colourTask.doEvents();
    lightMeterTask.doEvents();
    dhtTask.doEvents();
    accelGyroTask.doEvents();
    
    if (!display7SegTask.isBackgroundTask())
    {
        display7SegTask.doEvents();
    }

    displayLcdTask.doEvents();
    servoManager.doEvents();

#if defined(IR_REMOTE_ENABLED)
    if (!(config.i2CInUse() && config.mcuIsLeonardo()))
    {
        // Check for IR commands and send over serial.
        if (IrReceiver.decode())
        {
            if (IrReceiver.decodedIRData.protocol != UNKNOWN)
            {
                if (connection.isOpen())
                {
                    irMessage.setCommand(IrReceiver.decodedIRData.command);
                    irMessage.serialize(&MsgSerial);
                }
            }
            IrReceiver.resume();
        }
    }
#endif
}

void userISR()
{
    if (SONAR_ENABLED)
    {
        sonarTask.doISR();
    }
#if DEFAULT_MOTOR_DRIVER == MOTOR_DRIVER_DIRPWM || DEFAULT_MOTOR_DRIVER == MOTOR_DRIVER_ININ
    if (config.motorTaskAvailable())
    {
        config.getMotorTask()->doISR();
    }
#endif
    pulseCounterTask.doISR();

    if (LED_7SEG_ENABLED && display7SegTask.isBackgroundTask())
    {
        display7SegTask.doEvents();
    }
}

void onSettingI2CInUse()
{
    if (config.i2CInUse() && config.mcuIsLeonardo())
    {
#if defined(IR_REMOTE_ENABLED)
        IrReceiver.end();
#endif
    }
}

void beforeSonarPing()
{
    display7SegTask.runAsBackgroundTask(true);
}

void afterSonarPing()
{
    display7SegTask.runAsBackgroundTask(false);
}