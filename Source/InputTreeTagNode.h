#pragma once

#include "IInputTreeNode.h"
#include "JuceHeader.h"

class InputTreeTagNode : public IInputTreeNode {
  public:
    InputTreeTagNode(const juce::XmlElement& source);
    NoteContext& visit(NoteContext& context) override;

  private:
    std::string tag;
};