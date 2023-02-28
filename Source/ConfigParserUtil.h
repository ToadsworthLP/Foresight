#pragma once

#include "JuceHeader.h"

const int octaveForMiddleC = 3;

class ConfigParserUtil {
public:
	// Adapted from https://forum.juce.com/t/midimessage-keynametonumber/9904
	static int keyNameToNumber(const juce::String& keyName);
};