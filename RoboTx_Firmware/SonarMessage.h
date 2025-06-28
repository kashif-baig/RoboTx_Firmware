#ifndef SONAR_MESSAGE_H
#define SONAR_MESSAGE_H

#include <MessagingLib.h>
#include "SonarTask.h"

extern const char *SonarMessageType;

class SonarTask;

/**
 * Deserializes request to send a sonar ping.
 */
class SonarMessage : public SerializableMessage, public IDeserializableMessage
{
public:

    /**
     * Constructs instance with reference to SonarTask, which sends the ping and
     * serializes the distance as the response.
     */
    SonarMessage(SonarTask *sonarTask)
    {
        _sonarTask = sonarTask;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return SonarMessageType;
    }

    // Sets the distance to be serialized back to the client.
    void setDistanceCm(int16_t distanceCm)
    {
        _distanceCm = distanceCm;
    }

    void onDeserializeProperty(uint16_t propertyIndex, const char *propertyValue)
    {
        // This message has no incomming properties.
        // See implementation of onEndDeserialize().
    }

    /**
     * Processes the deserialized request.
     */
    void onEndDeserialize(bool messageComplete);

    void onSerialize()
    {
        serializeProperty("");
        getStream()->print(_distanceCm);
    }
    
private:
    SonarTask *_sonarTask;
    int16_t _distanceCm;
};


#endif