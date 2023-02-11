#pragma once

#include "IInputTreeNode.h"
#include "JuceHeader.h"

class InputTreeNodeFactory {
  public:
    static IInputTreeNode* make(const juce::XmlElement& source);
};