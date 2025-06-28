#include "SonarMessage.h"

const char *SonarMessageType = "R";

void SonarMessage::onEndDeserialize(bool messageComplete)
{
    if (messageComplete)
    {
        _sonarTask->ping();
    }
}