#pragma once

#include "JuceHeader.h"
#include "IInputTreeNode.h"

class InputTreeNodeFactory {
public:
	static IInputTreeNode* make(const juce::XmlElement& source);
};