#include "Display7SegTask.h"
#include "Arduino.h"
#include "Settings.h"

/* Segment byte maps for numbers 0 to 9 */
PROGMEM const uint8_t DT_SEGMENT_MAP_DIGIT[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0X80, 0X90};
/* Segment byte maps for alpha a-z */
PROGMEM const uint8_t DT_SEGMENT_MAP_ALPHA[] = {136, 131, 167, 161, 134, 142, 144, 139, 207, 241, 182, 199, 182, 171, 163, 140, 152, 175, 146, 135, 227, 182, 182, 182, 145, 182};

uint8_t DT_SEGMENT_SELECT[4] = {0xF1, 0xF2, 0xF4, 0xF8};

uint8_t AsciiToSegmentValue_DT(uint8_t ascii);

// ----------------------------------------------------------------------------------------------------

Display7SegTask::Display7SegTask(uint8_t latchPin, uint8_t clkPin, uint8_t dataPin)
{
    _displayMasterEnabled = 255;
    _latchPin = latchPin;
    _clkPin = clkPin;
    _dataPin = dataPin;

#if defined(AVR)
    uint8_t port;
    _latchPinPortbit = digitalPinToBitMask(_latchPin);
    port = digitalPinToPort(_latchPin);
    _latchPinPortReg = portOutputRegister(port);

    _clkPinPortbit = digitalPinToBitMask(_clkPin);
    port = digitalPinToPort(_clkPin);
    _clkPinPortReg = portOutputRegister(port);

    _dataPinPortbit = digitalPinToBitMask(_dataPin);
    port = digitalPinToPort(_dataPin);
    _dataPinPortReg = portOutputRegister(port);
#else

#endif
}

void Display7SegTask::setEnabled(bool enabled)
{
    if (LED_7SEG_ENABLED)
    {
        pinMode(_latchPin, OUTPUT);
        pinMode(_clkPin, OUTPUT);
        pinMode(_dataPin, OUTPUT);

        _displayMasterEnabled = 0;
        writeSegmentValue(0, 255);

        if (enabled)
        {
            setTime();
        }
        else
        {
            digitalWrite(_latchPin, LOW);
            digitalWrite(_clkPin, LOW);
            digitalWrite(_dataPin, LOW);
        }

        for (uint8_t i=0; i< sizeof(_displayMemory); i++)
        {
            _displayMemory[i] = 255;
        }
        _displayMasterEnabled = enabled;
    }
}

// ----------------------------------------------------------------------------------------------------

bool Display7SegTask::isEnabled() const
{
    return _displayMasterEnabled == ON;
}

// ----------------------------------------------------------------------------------------------------

void Display7SegTask::setSegmentRTL(bool setRTL)
{
    if (setRTL)
    {
        uint8_t seg_sel[] = {0xF8, 0xF4, 0xF2, 0xF1};

        for (uint8_t i = 0; i < sizeof(seg_sel); i++)
        {
            DT_SEGMENT_SELECT[i] = seg_sel[i];
        }
    }
    else
    {
        uint8_t seg_sel[] = {0xF1, 0xF2, 0xF4, 0xF8};

        for (uint8_t i = 0; i < sizeof(seg_sel); i++)
        {
            DT_SEGMENT_SELECT[i] = seg_sel[i];
        }
    }
}

// ----------------------------------------------------------------------------------------------------

void Display7SegTask::setDisplayInverse(bool isInverse)
{
    _displayXor = isInverse ? 255 : 0;
}

// ----------------------------------------------------------------------------------------------------

void Display7SegTask::setDisplayBrightness(uint8_t level)
{
    _displayBrightness = level >= DISPLAY_TIMER_SCALER_RELOAD ? DISPLAY_TIMER_SCALER_RELOAD - 1 : level;
}

// ----------------------------------------------------------------------------------------------------

void Display7SegTask::write(const char *text, uint8_t rightAlign)
{
    uint8_t displayBuf[] = {0, 0, 0, 0}, *pBuf = displayBuf;

    uint8_t idx = 0;

    for (; *text != 0 && idx < sizeof(displayBuf); text++)
    {
        uint8_t offset = 0;

        if (*text == '.')
        {
            if (idx > 0)
            {
                displayBuf[idx - 1] = displayBuf[idx - 1] & 127;
            }
            else
            {
                displayBuf[idx] = AsciiToSegmentValue_DT(*text);
                idx++;
            }
        }
        else
        {
            displayBuf[idx] = AsciiToSegmentValue_DT(*text);
            idx++;
        }
    }

    for (; idx < sizeof(displayBuf); idx++)
    {
        // TODO need to apply inverse if set.
        displayBuf[idx] = 255;
    }

    // Copy display buffer to display memory

    for (uint8_t i = 0; i < sizeof(displayBuf); i++)
    {
        _displayMemory[i] = displayBuf[i];
    }
}

// ----------------------------------------------------------------------------------------------------

// void Display7SegTask::blinkDisplay(uint8_t digits, uint8_t enabled)
// {
// }

// ----------------------------------------------------------------------------------------------------

void Display7SegTask::doEvents()
{
    uint8_t displayEnabled = 1;

    _displayTimerScaler--;

    if (_displayTimerScaler == 0)
    {
        _displayTimerScaler = DISPLAY_TIMER_SCALER_RELOAD;

        // Digit display blink control
        if (!_displayMasterEnabled)
        {
            // DO nothing.
        }
        else
        {
            writeSegmentValue(_displayIdx, _displayMemory[_displayIdx]);
        }

        _displayIdx++;
        if (_displayIdx > sizeof(_displayMemory) - 1)
        {
            _displayIdx = 0;
        }
    }
    else if (_displayMasterEnabled)
    {
        // Handle display brightness
        if (_displayTimerScaler == _displayBrightness)
        {
            writeSegmentValue(_displayIdx == 0 ? 3 : _displayIdx - 1, 255);
        }
    }
}

// ----------------------------------------------------------------------------------------------------

#if defined(AVR)
void Display7SegTask::writeSegmentValue(uint8_t Segment, uint8_t Value)
{
    if (LED_7SEG_ENABLED)
    {
        Value = Value ^ _displayXor;
        
        writePortAVR(_latchPinPortReg, _latchPinPortbit, LOW);

        for (uint8_t i = 0; i < 8; i++) {
            writePortAVR(_dataPinPortReg, _dataPinPortbit, !!(Value & (1 << (7 - i))));
            writePortAVR(_clkPinPortReg, _clkPinPortbit, HIGH);
            writePortAVR(_clkPinPortReg, _clkPinPortbit, LOW);
        }

        for (uint8_t i = 0; i < 8; i++) {
            writePortAVR(_dataPinPortReg, _dataPinPortbit, !!(DT_SEGMENT_SELECT[Segment] & (1 << (7 - i))));
            writePortAVR(_clkPinPortReg, _clkPinPortbit, HIGH);
            writePortAVR(_clkPinPortReg, _clkPinPortbit, LOW);
        }

        writePortAVR(_latchPinPortReg, _latchPinPortbit, HIGH);
    }
}
#else
void Display7SegTask::writeSegmentValue(uint8_t Segment, uint8_t Value)
{
    if (LED_7SEG_ENABLED)
    {
        digitalWrite(_latchPin, LOW);
        shiftOut(_dataPin, _clkPin, MSBFIRST, Value ^ _displayXor);
        shiftOut(_dataPin, _clkPin, MSBFIRST, DT_SEGMENT_SELECT[Segment]);
        digitalWrite(_latchPin, HIGH);
    }
}
#endif

// ----------------------------------------------------------------------------------------------------

uint8_t AsciiToSegmentValue_DT(uint8_t ascii)
{
    uint8_t segmentValue = 182;

    if (ascii >= '0' && ascii <= '9')
    {
        segmentValue = pgm_read_byte(&DT_SEGMENT_MAP_DIGIT[ascii - '0']);
    }
    else if (ascii >= 'a' && ascii <= 'z')
    {
        segmentValue = pgm_read_byte(&DT_SEGMENT_MAP_ALPHA[ascii - 'a']);
    }
    else if (ascii >= 'A' && ascii <= 'Z')
    {
        segmentValue = pgm_read_byte(&DT_SEGMENT_MAP_ALPHA[ascii - 'A']);
    }
    else
    {
        switch (ascii)
        {
        case '-':
            segmentValue = 191;
            break;
        case '.':
            segmentValue = 127;
            break;
        case '_':
            segmentValue = 247;
            break;
        case ' ':
            segmentValue = 255;
            break;
        }
    }

    return segmentValue;
}

/* ---------------------------------------------------------------------- */
