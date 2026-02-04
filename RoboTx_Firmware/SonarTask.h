#ifndef SONAR_TASK_H
#define SONAR_TASK_H

#include "Task.h"
#include <MessagingLib.h>
#include "SonarMessage.h"
#include "Settings.h"

class SonarMessage;

/**
 * Responsible for sending a sonar ping, waiting for the echo, then serializing
 * the calculated distance via the message sender.
 * The sonar trigger pin must be any available Arduino digital pin. The echo pin
 * can be any available digital or analog pin. If the latter, the same analog pin
 * must not be enabled for any other use.
 */
class SonarTask
{
public:
   SonarTask(uint8_t trigPin, uint8_t echoPin)
    : _trigPin(trigPin), _echoPin(echoPin)
   {
   }

   // Inialises the trigger and echo pins.
   void initialize()
   {
      if (SONAR_ENABLED)
      {
         pinMode(_trigPin, OUTPUT);
         pinMode(_echoPin, INPUT);

         digitalWrite(_trigPin, LOW);

         _bit = digitalPinToBitMask(_echoPin);
          uint8_t port = digitalPinToPort(_echoPin);
         _portInpReg = portInputRegister(port);
      }
   }

   /**
    * Establishes a reference to the SonarMessage that will serialize the calculated distance.
    */
   void setSonarMessage(SonarMessage *sonarMessage)
   {
       _sonarMessage = sonarMessage;
   }

   // Sonar echo timeout implemented using interrupt service routine that runs at 1000 hz.
   void doISR()
   {
      if (_sonarTimeoutMs)
         _sonarTimeoutMs--;
   }
   
   /**
    * Sends sonar ping, waits for echo, calculates distance, serializes distance value.
    */
   void ping();

   // Callback invoked just before the actual ping.
   void (* onBeforePing)() = 0;
   // Callback invoked just after the actual ping.
   void (* onAfterPing)() = 0;

private:
   const uint8_t _trigPin;
   const uint8_t _echoPin;

   uint8_t _bit;

#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
   const volatile uint16_t *_portInpReg;
#else
   volatile uint8_t *_portInpReg;
#endif

   volatile uint8_t _sonarTimeoutMs = 0;

   SonarMessage *_sonarMessage;
};

#endif