#include "SerializableMessageLite.h"

void SerializableMessageLite::serialize(Print *outStream)
{
    this->stream = outStream;

    stream->write(MSG_START);
    stream->write(getMessageType());
    onSerialize();
    stream->write(MSG_END);
}


void SerializableMessageLite::serializeProperty(const char *propertyValue)
{
    stream->write(MSG_PROPERTY);
    stream->print(propertyValue);
}


Print *SerializableMessageLite::getStream() const
{
    return stream;
}
