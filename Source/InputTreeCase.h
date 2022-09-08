#pragma once

#include "JuceHeader.h"

class InputTreeCase {
public:
	InputTreeCase(const juce::XmlElement& source);
	bool check(int value);
private:
	bool alwaysTrue;
	int equals, greater, less;
};