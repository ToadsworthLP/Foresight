#include "ConfigParserUtil.h"

int ConfigParserUtil::keyNameToNumber(const juce::String& keyName)
{
    static const char* const noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "", "Db", "", "Eb", "", "", "Gb", "", "Ab", "", "Bb" };

    int keyNumber, octave = 0, numPos = keyName.indexOfAnyOf("01234567890-");

    if (numPos == 0)
        keyNumber = keyName.getIntValue(); //apparently already a number!
    else
    {
        if (numPos > 0)
        {
            octave = keyName.substring(numPos).getIntValue() - octaveForMiddleC + 5;
        }
        else
        {
            octave = octaveForMiddleC; //default to octave of middle C if none found
            numPos = keyName.length();
        }

        juce::String name(keyName.substring(0, numPos).trim().toUpperCase());

        keyNumber = juce::StringArray(noteNames, 12).indexOf(name) % 12;

        if (keyNumber < 0) {
            throw std::runtime_error("Encountered invalid note name in <set> tag.");
        }
    }

    return keyNumber + octave * 12;
}
