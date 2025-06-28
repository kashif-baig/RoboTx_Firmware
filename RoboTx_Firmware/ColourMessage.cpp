#include "ColourMessage.h"

const char *ColourMessageType = "L";

void ColourMessage::onEndDeserialize(bool messageComplete)
{
    if (messageComplete)
    {
        if (_enableRequested)
        {
            _colourTask->enable(_integrationTimeIdx, _gain);
        }
        else
        {
            _colourTask->disable();
        }
    }
}


void ColourMessage::onSerialize()
{
    serializeToHex(_colourTask->getR());
    serializeToHex(_colourTask->getG());
    serializeToHex(_colourTask->getB());
    serializeToHex(_colourTask->getClear());
}