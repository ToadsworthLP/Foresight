#pragma once

#include "JuceHeader.h"

class InputTreeCase {
public:
	InputTreeCase(const juce::XmlElement& source, int keyswitch);
	bool check(int value);
private:
	bool alwaysTrue = false;
	int equals = -1;
	int greater = INT_MIN;
	int less = INT_MAX;
};