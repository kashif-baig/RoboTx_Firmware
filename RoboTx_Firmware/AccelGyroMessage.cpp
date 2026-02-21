#include "AccelGyroMessage.h"

const char *AccelGyroMessageType = "X";

void AccelGyroMessage::onSerialize()
{
    switch (_valueType)
    {
    case 'A':
    case 'G':
    {
        char valType[2] = {_valueType, 0};

        serializeProperty(valType);
        serializeToHex(_x);
        serializeToHex(_y);
        serializeToHex(_z);
        break;
    }
    }
    _valueType = 0;
}
