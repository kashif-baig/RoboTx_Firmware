#ifndef SERIALIZABLE_MESSAGE_LITE_H
#define SERIALIZABLE_MESSAGE_LITE_H

#include <Arduino.h>
#include <MessagingLib.h>

// An abstract class that supports serialization of simple properties.
class SerializableMessageLite
{
public:
    // Returns the message type string. Must be unique for each implementation.
    virtual const char *getMessageType() = 0;

    // Serializes object to the stream.
    void serialize(Print *outStream);

protected:

    // An abstract method that serializes the object.
    virtual void onSerialize() = 0;

    // Serializes a property to the stream. Set the propertyValue
    // to an empty string and use the underlying stream if more
    // formatting control of the property value is required.
    void serializeProperty(const char *propertyValue);

    // Gets the underlying stream.
    Print *getStream() const;

private:
    Print *stream;
};

#endif