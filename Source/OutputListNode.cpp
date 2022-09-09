#include "OutputListNode.h"

OutputListNode::OutputListNode()
{
    target = END;
    value = 0;
}

OutputListNode::OutputListNode(const juce::XmlElement& source)
{
	std::string targetStr = source.getStringAttribute("target").toStdString();
	std::string valueStr = source.getAllSubText().toStdString();

    if (targetStr == "note") target = NOTE;
    else if (targetStr == "start") target = START;
    else if (targetStr == "end") target = END;
    else {
        if (targetStr.starts_with("CC")) {
            std::string trimmed = targetStr.substr(2, targetStr.length());
            ccNumber = std::stoi(trimmed);

            target = CC;
        }
        else {
            throw std::exception("Encountered a <set> node target attribute with an invalid value.");
        }
    }

    if (target == NOTE) {
        value = keyNameToNumber(juce::String(valueStr), 3);
    }
    else {
        value = std::stoi(valueStr);
    }
}

OutputListNode::TargetType OutputListNode::getTargetType()
{
    return target;
}

int OutputListNode::getCCNumber()
{
    return ccNumber;
}

int OutputListNode::getValue()
{
    return value;
}

int OutputListNode::keyNameToNumber(const juce::String& keyName, const int octaveForMiddleC) {

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
            throw std::exception("Encountered invalid note name in <set> tag.");
        }
    }

    return keyNumber + octave * 12;
}