#pragma once

#include "JuceHeader.h"
#include "BufferedNote.h"

class OutputListNode {
public:
	OutputListNode();
	OutputListNode(const juce::XmlElement& source);

	enum TargetType { CC, NOTE, START, END };
	TargetType getTargetType();
	int getCCNumber();
	int getValue();

private:

	TargetType target;
	int ccNumber = 0;
	int value = 0;

	// Adapted from https://forum.juce.com/t/midimessage-keynametonumber/9904
	int keyNameToNumber(const juce::String& keyName, const int octaveForMiddleC);
};