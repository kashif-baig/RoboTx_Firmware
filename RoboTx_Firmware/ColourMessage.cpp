#include "ColourMessage.h"

const char *ColourMessageType = "L";

void ColourMessage::onSerialize()
{
    serializeToHex(_colourTask->getR());
    serializeToHex(_colourTask->getG());
    serializeToHex(_colourTask->getB());
    serializeToHex(_colourTask->getClear());
}