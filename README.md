# Robo‑Tx Firmware for Arduino

The Robo‑Tx firmware is a modular Arduino C++ codebase that implements a message/task architecture for remote interaction (via serial communication) with sensors and actuators from a client computer, e.g. PC. The code includes sensor drivers, message classes, task handlers, motor/servo managers, I2C helpers, and serialization utilities; the project's entry point is RoboTx_Firmware.ino. It targets Arduino R3/R4 microcontroller boards and kits. Be sure to review library dependencies listed in the .ino file and configuration in Settings.h before deploying.

## Overview - what the repository contains

- **Main sketch:** RoboTx_Firmware.ino is the firmware entry point.
- **File organization:** code is split into header (.h) and implementation (.cpp) pairs for each feature (tasks, messages, drivers, utilities).

## Core architectural features

- **Message / Serialization layer:** Multiple \*Message.\* files implement lightweight message types and serialization for communication with the Robo-Tx API deployed to a client computer, e.g. PC. This enables structured messages for remote control of sensors, motors, displays, and connection events. Review classes of Arduino library MessagingLib for more information.
- **Task abstraction:** Task.h plus many \*Task.\* files (e.g., DHTTask, SonarTask, LightMeterTask, Display7SegTask) implement periodic or event‑driven tasks that encapsulate sensor polling and actuator control. Tasks separate hardware polling from higher‑level logic.
- **Driver wrappers:** sensor drivers like BH1750_Lite, DHT20_Lite, MPU6050_Lite, TCS34725_Lite provide compact interfaces to common I2C sensors. These wrappers simplify I2C integration and usage.

## Hardware and peripheral support

- **Sensors:** light (BH1750), temperature/humidity (DHT20), IMU (MPU6050), colour sensor (TCS34725), sonar (HC‑SR04 style via SonarTask).
- **Actuators and motor control:** ServoManager.h, MotorSpeedController.h, and multiple motor task headers (e.g., MotorDirPwmTask.h, MotorHW56Task.h) implement support for servos and different motor driver peripherals.
- **User I/O and utilities:** 7‑segment and LCD displays, beeper, buttons, pulse counter, analog inputs, and a SwitchManager class for digital output handling.

## Communication and system utilities

- **I2C abstraction:** I2C.cpp/.h and I2CRequestMessage.\* files centralize I2C transactions.
- **Connection and debug helpers:** Connection.h, ConnectionMessage.h, Debug.cpp/.h provide connection state handling and debug output.
- **Configuration:** Config.\*, ConfigMessage.h and Settings.\* manage runtime settings as well as compile time configuration settings.

## Extensibility and developer guidance

- **Modular pattern:** to add new sensor or actuator support, create a \*Message.h/.cpp pair and a \*Task.h/.cpp that implements the Task interface; register it from the main .ino file. Review implementations of Task.h, IDeserializableMessage, SerializableMessageLite.\*, and MyDeserializableMessageFactory.h for extension points.

### Practical notes and limitations

- **Platform:** the firmware requires Arduino‑compatible toolchains (Arduino IDE / Visual Studio Code) and Arduino OS drivers.
- **Inspection recommended:** review RoboTx_Firmware.ino, Settings.h, and Arduino library MessagingLib to understand task instantiation and scheduling, peripheral configuration, and messaging, before attempting further hardware integration.
- **Memory limitations:** if there is insufficient program or RAM available on the target Arduino platform, e.g. Uno R3, consider disabling features in the Settings.h file.