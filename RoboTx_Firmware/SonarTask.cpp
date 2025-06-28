#include "SonarTask.h"
#include <Arduino.h>

void SonarTask::ping()
{
    if (SONAR_ENABLED)
    {
        if (onBeforePing)
        {
            onBeforePing();
        }

        digitalWriteAlt(_trigPin, HIGH);
        delayMicroseconds(10);
        digitalWriteAlt(_trigPin, LOW);

        _sonarTimeoutMs = 2;

        // wait for any previous echo pulse to end
        while ((*_portInpReg & _bit))
        {
            if (!_sonarTimeoutMs)
            {
                if (onAfterPing)
                {
                    onAfterPing();
                }    
                return;
            }
        }

        _sonarTimeoutMs = 2;

        // wait for the echo pulse to start
        while (!(*_portInpReg & _bit))
        {
            if (!_sonarTimeoutMs)
            {
                if (onAfterPing)
                {
                    onAfterPing();
                }    
                return;
            }
        }

        //_sonarTimeoutMs = 30; // 
        _sonarTimeoutMs = 10; // limit sonar range to 1.5 meters approx.

        uint32_t timeStart = micros();

        // wait for the echo pulse to stop
        while ((*_portInpReg & _bit))
        {
            if (!_sonarTimeoutMs)
            {
                if (onAfterPing)
                {
                    onAfterPing();
                }    
                return;
            }
        }

        int16_t distanceCm = ((micros() - timeStart) * 141) >> 13;

        if (!_messageSender->messageQueued(_sonarMessage))
        {
            // Queue the sonar message with calculated distance, then send.
            _sonarMessage->setDistanceCm(distanceCm);
            _messageSender->queueMessage(_sonarMessage);
        }

        if (onAfterPing)
        {
            onAfterPing();
        }    
    }
}