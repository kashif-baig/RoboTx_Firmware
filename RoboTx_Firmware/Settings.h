#ifndef _SETTINGS_H
#define _SETTINGS_H

// Uncomment line below if using Bluetooth
//#define BLUETOOTH_ROBOT

#define SERIAL_BAUD 115200
//#define DEBUG_ENABLED 1

#include <Arduino.h>

#if defined (BLUETOOTH_ROBOT) && (defined(__AVR_ATmega32U4__) || defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI))
    #define MsgSerial Serial1
#else
    #define MsgSerial Serial
#endif

// A user defined Id string.
#define ROBOT_ID    "Robo-Tx robot"

// ----------------------------------------------------------------------------

// Commenting out any of the lines immediately below makes the feature unavailable.
#define IR_REMOTE_ENABLED
#define COLOUR_SENSING_ENABLED
#define DISPLAY_LCD_ENABLED

// ----------------------------------------------------------------------------
/**
 * Firmware Profile Pin Assignments.
 * 
 * Arduino pins 0 to 2 are reserved, and A0 to A7 are intended for analog/digital input.
 * Avoid using analog pins for motors, servos, LED display, digital trigger or sonar trigger.
 * 
 * By default digital input pins are from A0 to A4. However, a maximum of five input pins
 * can be defined as comma separated text using the macro name DIGITAL_INPUT_PINS.
 * 
 * Analog pin A2 is used for pulse counter input (when enabled). Uno A4 and A5 and
 * Leonardo D2 and D3 are used for I2C devices (when enabled).
 * 
 * It's usually best to avoid overlapping pin assignments. In some cases pin assignments
 * can overlap, so ensure only one of the devices is in active use via API software.
 * 
 * Where software PWM is used, any available digital pin can be used for driving motors.
 * For consistency in vehicles, motor 1 should be the left hand motor, motor 2 the right hand motor.
 * 
 */
// ----------------------------------------------------------------------------

// Firmware profile identifiers. See further below for profile pin assignments.
#define PROFILE_MULTI_FUNC_SHIELD   1
#define PROFILE_L298_BREAKOUT_BOARD 2
#define PROFILE_ROBOT_MOTOR_SHIELD  3
#define PROFILE_UNO_MOTOR_PLUS      4
#define PROFILE_ARDU_MOTOR_SHIELD   5
#define PROFILE_ROBOT_ARM_4DF       6
#define PROFILE_POLOLU_3PI          7

#define SELECTED_PROFILE        PROFILE_MULTI_FUNC_SHIELD

// ----------------------------------------------------------------------------

// Motor driver profile identifiers.

// Soft PWM where motor driver has two PWM inputs for each motor, e.g IN1 and IN2.
#define MOTOR_DRIVER_ININ      1
// Soft PWM where motor driver has a direction input and a PWM input for each motor.
#define MOTOR_DRIVER_DIRPWM    2
// Hardware PWM where motor driver has a direction input and a PWM input for each motor,
// and Ardiuno pins 5 and 6 used for PWM.
#define MOTOR_DRIVER_HW56      4

// ----------------------------------------------------------------------------

// Arduino pin assignments are defined for each profile below.

#if SELECTED_PROFILE == PROFILE_MULTI_FUNC_SHIELD

    #define ANALOG_ENABLED_A            0,4,5       /* A0, A4, A5 */
    #define DIGITAL_INPUT_PINS          A1,A2,A3    /* actual Arduino pins */
    #define DIGITAL_INPUTS_ENABLED      0,1,2       /* index positions that map to digital pins defined above */
    #define DIGITAL_INPUTS_INVERTED     0,1,2       /* index positions that map to digital pins defined above */

    #define DEFAULT_MOTOR_DRIVER    MOTOR_DRIVER_DIRPWM

    #define LED_7SEG_ENABLED            true
    #define LED_7SEG_RTL                false   /* Enable/disable Right to Left display order */
    #define LED_7SEG_LATCH_PIN           4
    #define LED_7SEG_CLK_PIN             7
    #define LED_7SEG_DATA_PIN            8

    #define SONAR_ENABLED               true
    #define SONAR_TRIG_PIN               6
    #define SONAR_ECHO_PIN               9

    #define SERVO1_ENABLED              true
    #define SERVO1_PIN                   5
    #define SERVO2_ENABLED              true
    #define SERVO2_PIN                   6

    #define SWITCH1_ENABLED             true    /* Switches used for LED on/off control */
    #define SWITCH1_ON_VALUE            LOW
    #define SWITCH1_PIN                 13
    #define SWITCH2_ENABLED             true
    #define SWITCH2_ON_VALUE            LOW
    #define SWITCH2_PIN                 12
    #define SWITCH3_ENABLED             true
    #define SWITCH3_ON_VALUE            LOW
    #define SWITCH3_PIN                 11
    #define SWITCH4_ENABLED             true
    #define SWITCH4_ON_VALUE            LOW
    #define SWITCH4_PIN                 10

    #define TRIGGER_ENABLED             true    /* trigger used for beeper control */
    #define TRIGGER_PIN                  3
    #define TRIGGER_ON_VALUE            LOW

#elif SELECTED_PROFILE == PROFILE_ROBOT_MOTOR_SHIELD

    #define DEFAULT_MOTOR_DRIVER    MOTOR_DRIVER_DIRPWM

    #define MOTOR1_DIRPWM_PWM_PIN      10
    #define MOTOR1_DIRPWM_DIR_PIN      12
    #define MOTOR2_DIRPWM_PWM_PIN      11
    #define MOTOR2_DIRPWM_DIR_PIN      13
    #define MOTOR1_DIRPWM_INV_DIR      false   /* Set to true to invert motor 1 direction */
    #define MOTOR2_DIRPWM_INV_DIR      false   /* Set to true to invert motor 2 direction */

    #define LED_7SEG_ENABLED            true
    #define LED_7SEG_RTL                true   /* Enable/disable Right to Left display order */
    #define LED_7SEG_DATA_PIN            3
    #define LED_7SEG_LATCH_PIN           5
    #define LED_7SEG_CLK_PIN             6

    #define SONAR_ENABLED               true
    #define SONAR_TRIG_PIN               7
    #define SONAR_ECHO_PIN               8

    #define SERVO1_ENABLED              true
    #define SERVO1_PIN                   9

    #define SWITCH1_ENABLED             true
    #define SWITCH1_ON_VALUE            HIGH
    #define SWITCH1_PIN                 3

    #define SWITCH2_ENABLED             true
    #define SWITCH2_ON_VALUE            HIGH
    #define SWITCH2_PIN                 5

    #define SWITCH3_ENABLED             true
    #define SWITCH3_ON_VALUE            HIGH
    #define SWITCH3_PIN                 6

    #define TRIGGER_ENABLED             true
    #define TRIGGER_PIN                  4
    #define TRIGGER_ON_VALUE            HIGH

#elif SELECTED_PROFILE == PROFILE_UNO_MOTOR_PLUS

    #define DEFAULT_MOTOR_DRIVER    MOTOR_DRIVER_HW56

    #define MOTOR1_HW56_DIR1_PIN         4
    #define MOTOR2_HW56_DIR1_PIN         7
    #define MOTOR1_HW56_INV_DIR        false
    #define MOTOR2_HW56_INV_DIR        false

    #define SONAR_ENABLED               true
    #define SONAR_TRIG_PIN               8
    #define SONAR_ECHO_PIN               9

    #define SERVO1_ENABLED              true
    #define SERVO1_PIN                  12
    #define SERVO2_ENABLED              true
    #define SERVO2_PIN                  13

    #define SWITCH1_ENABLED             true
    #define SWITCH1_ON_VALUE            HIGH
    #define SWITCH1_PIN                 10

    #define SWITCH2_ENABLED             true
    #define SWITCH2_ON_VALUE            HIGH
    #define SWITCH2_PIN                 11

    #define TRIGGER_ENABLED             true
    #define TRIGGER_PIN                  3
    #define TRIGGER_ON_VALUE            LOW

#elif SELECTED_PROFILE == PROFILE_ARDU_MOTOR_SHIELD

    #define DEFAULT_MOTOR_DRIVER    MOTOR_DRIVER_DIRPWM

    #define MOTOR1_DIRPWM_PWM_PIN        3
    #define MOTOR1_DIRPWM_DIR_PIN       12
    #define MOTOR2_DIRPWM_PWM_PIN       11
    #define MOTOR2_DIRPWM_DIR_PIN       13
    #define MOTOR1_DIRPWM_INV_DIR      false   /* Set to true to invert motor 1 direction */
    #define MOTOR2_DIRPWM_INV_DIR      false   /* Set to true to invert motor 2 direction */
    #define MOTOR_DIRPWM_EN_DEFAULT    false   /* Set to true to use motor brake pins by default */
    #define MOTOR_DIRPWM_ENABLE_VALUE  LOW     /* value to use to enable a motor */
    #define MOTOR1_DIRPWM_EN_PIN         9     /* Assigning an enable pin only for Motor1 means it can act as master enable for both motors. */
    #define MOTOR2_DIRPWM_EN_PIN         8     /* -1 means unassigned */

    #define LED_7SEG_ENABLED            true
    #define LED_7SEG_RTL                false   /* Enable/disable Right to Left display order */
    #define LED_7SEG_LATCH_PIN          11
    #define LED_7SEG_CLK_PIN            12
    #define LED_7SEG_DATA_PIN           13

    #define SONAR_ENABLED               true
    #define SONAR_TRIG_PIN               5
    #define SONAR_ECHO_PIN              A3      /* Assigning an analog pin for the Sonar echo is OK. Don't use for anything else.*/

    #define SERVO1_ENABLED              true
    #define SERVO1_PIN                   7

    #define SWITCH1_ENABLED             true
    #define SWITCH1_ON_VALUE            HIGH
    #define SWITCH1_PIN                  5
    #define SWITCH2_ENABLED             true
    #define SWITCH2_ON_VALUE            HIGH
    #define SWITCH2_PIN                  4

    #define TRIGGER_ENABLED             true
    #define TRIGGER_PIN                  6
    #define TRIGGER_ON_VALUE            LOW

#elif SELECTED_PROFILE == PROFILE_L298_BREAKOUT_BOARD

    #define DEFAULT_MOTOR_DRIVER    MOTOR_DRIVER_HW56

    #define MOTOR1_HW56_DIR1_PIN        4
    #define MOTOR1_HW56_DIR2_PIN        3
    #define MOTOR1_HW56_IDLE_ENABLED   true
    #define MOTOR1_HW56_INV_DIR        false

    #define MOTOR2_HW56_DIR1_PIN        7
    #define MOTOR2_HW56_DIR2_PIN        8
    #define MOTOR2_HW56_IDLE_ENABLED   true
    #define MOTOR2_HW56_INV_DIR        false

    #define SONAR_ENABLED              true
    #define SONAR_TRIG_PIN              10
    #define SONAR_ECHO_PIN              11

    #define SERVO1_ENABLED             true
    #define SERVO1_PIN                  12
    #define SERVO2_ENABLED             true
    #define SERVO2_PIN                  13

    #define TRIGGER_ENABLED            true
    #define TRIGGER_PIN                  9
    #define TRIGGER_ON_VALUE            LOW

#elif SELECTED_PROFILE == PROFILE_ROBOT_ARM_4DF

    #define ANALOG_ENABLED_A            3,4,5   /* A3, A4, A5 */
    #define DIGITAL_INPUTS_ENABLED      0,1,2   /* A0, A2, A3 */

    #define DEFAULT_MOTOR_DRIVER    MOTOR_DRIVER_HW56

    #define MOTOR1_HW56_DIR1_PIN        4
    #define MOTOR2_HW56_DIR1_PIN        7
    #define MOTOR1_HW56_INV_DIR        false
    #define MOTOR2_HW56_INV_DIR        false
    
    #define SONAR_ENABLED               true
    #define SONAR_TRIG_PIN              12
    #define SONAR_ECHO_PIN              13

    #define SERVO1_ENABLED              true
    #define SERVO1_PIN                   8
    #define SERVO2_ENABLED              true
    #define SERVO2_PIN                   9
    #define SERVO3_ENABLED              true
    #define SERVO3_PIN                  10
    #define SERVO4_ENABLED              true
    #define SERVO4_PIN                  11

    #define SWITCH1_ENABLED             true
    #define SWITCH1_ON_VALUE            HIGH
    #define SWITCH1_PIN                  6
    #define SWITCH2_ENABLED             true
    #define SWITCH2_ON_VALUE            HIGH
    #define SWITCH2_PIN                  7

    #define TRIGGER_ENABLED             true
    #define TRIGGER_PIN                  3
    #define TRIGGER_ON_VALUE            LOW
    
#elif SELECTED_PROFILE == PROFILE_POLOLU_3PI

    #define ANALOG_ENABLED_A            6,7         /* A6 and A7*/
    #define DIGITAL_INPUT_PINS          9,12,13     /* actual Arduino pins */
    #define DIGITAL_INPUTS_ENABLED      0,1,2       /* index positions that map to the actual pins */
    #define DIGITAL_INPUTS_INVERTED     0,1,2       /* index positions that map to the actual pins */

    #define DEFAULT_MOTOR_DRIVER    MOTOR_DRIVER_ININ

    #define MOTOR1_ININ_IDLE_ENABLED    true    /* Set to true to enable idling/coasting */
    #define MOTOR1_ININ_FWD_PIN          6
    #define MOTOR1_ININ_REV_PIN          5
    #define MOTOR2_ININ_IDLE_ENABLED    true    /* Set to true to enable idling/coasting */
    #define MOTOR2_ININ_FWD_PIN         11
    #define MOTOR2_ININ_REV_PIN          3

    #define SWITCH1_ENABLED             true
    #define SWITCH1_ON_VALUE            HIGH
    #define SWITCH1_PIN                  A5

    #define TRIGGER_ENABLED             true
    #define TRIGGER_PIN                  7
    #define TRIGGER_ON_VALUE            HIGH
#endif

// ----------------------------------------------------------------------------

// I2C LCD settings
#define DISPLAY_LCD_I2C_ADDR      0x27
#define DISPLAY_LCD_COLS            16
#define DISPLAY_LCD_ROWS             2
#define DISPLAY_LCD_LIGHT_DURATION  10

//----------------------------------------------------------------------------

// Default profile settings if not set in the profile sections above.

#ifndef LED_7SEG_ENABLED
    #define LED_7SEG_ENABLED            false
    #define LED_7SEG_RTL                false   /* Enable/disable Right to Left display order */
    #define LED_7SEG_LATCH_PIN           4
    #define LED_7SEG_CLK_PIN             7
    #define LED_7SEG_DATA_PIN            8
#endif

#ifndef SONAR_ENABLED
    #define SONAR_ENABLED               false
    #define SONAR_TRIG_PIN               6
    #define SONAR_ECHO_PIN               9
#endif

#ifndef SERVO1_ENABLED
    #define SERVO1_ENABLED              false
    #define SERVO1_PIN                   5
#endif
#ifndef SERVO2_ENABLED
    #define SERVO2_ENABLED              false
    #define SERVO2_PIN                   6
#endif
#ifndef SERVO3_ENABLED
    #define SERVO3_ENABLED              false
    #define SERVO3_PIN                   7
#endif
#ifndef SERVO4_ENABLED
    #define SERVO4_ENABLED              false
    #define SERVO4_PIN                   8
#endif

#ifndef SWITCH1_ENABLED
    #define SWITCH1_ENABLED             false
    #define SWITCH1_ON_VALUE            HIGH
    #define SWITCH1_PIN                  4
#endif
#ifndef SWITCH2_ENABLED
    #define SWITCH2_ENABLED             false
    #define SWITCH2_ON_VALUE            HIGH
    #define SWITCH2_PIN                  5
#endif
#ifndef SWITCH3_ENABLED
    #define SWITCH3_ENABLED             false
    #define SWITCH3_ON_VALUE            HIGH
    #define SWITCH3_PIN                  7
#endif
#ifndef SWITCH4_ENABLED
    #define SWITCH4_ENABLED             false
    #define SWITCH4_ON_VALUE            HIGH
    #define SWITCH4_PIN                  6
#endif

#ifndef TRIGGER_ENABLED
    #define TRIGGER_ENABLED             false
    #define TRIGGER_PIN                  3
    #define TRIGGER_ON_VALUE            LOW
#endif

//----------------------------------------------------------------------------

PROGMEM const char _HexStr[] = "0123456789ABCDEF";

#if defined(AVR)
    #define digitalWriteAlt(pin,val)    digitalWriteFast_(pin,val)
#else
    #define digitalWriteAlt(pin,val)    digitalWrite(pin,val)
#endif

#define writePortAVR(reg, bit, val) \
{                               \
    uint8_t _oldSREG = SREG;    \
    cli();                      \
    if (val == LOW) {           \
        *reg &= ~bit;           \
    }                           \
    else {                      \
        *reg |= bit;            \
    }                           \
    SREG = _oldSREG;            \
}

extern void digitalWriteFast_(uint8_t pin, uint8_t val);

extern void writeToStream(Print *response, const char *str_P);

#endif