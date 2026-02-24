#include "ColourMessage.h"

const char *ColourMessageType = "L";

void ColourMessage::onSerialize()
{
    serializeProperty(convertToHex(_colourTask->getR()));
    serializeProperty(convertToHex(_colourTask->getG()));
    serializeProperty(convertToHex(_colourTask->getB()));
    serializeProperty(convertToHex(_colourTask->getClear()));
}