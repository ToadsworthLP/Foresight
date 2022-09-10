#pragma once

#include "JuceHeader.h"

class ConfigParserUtil {
public:
	 // Adapted from https://forum.juce.com/t/midimessage-keynametonumber/9904
	static int keyNameToNumber(const juce::String& keyName, const int octaveForMiddleC);
};