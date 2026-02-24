#ifndef COLOUR_MESSAGE_H
#define COLOUR_MESSAGE_H

#include "ColourTask.h"
#include <MessagingLib.h>
#include "SerializableMessageLite.h"
#include "Settings.h"

class ColourTask;

extern const char *ColourMessageType;

/**
 * Deserializes request for configuring colour sensor.
 * Serializes response with colour values.
 */
class ColourMessage : public SerializableMessageLite
{
public:

    ColourMessage(ColourTask *colourTask)
    {
        _colourTask = colourTask;
    }

    /**
     * Gets a unique string that identifies the type of the message.
     */
    const char *getMessageType()
    {
        return ColourMessageType;
    }

protected:

    /**
     * Serializes the values read by the colour sensor.
     */
    void onSerialize();

private:
    ColourTask *_colourTask;
};

#endif