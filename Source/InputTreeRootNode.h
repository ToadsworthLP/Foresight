#pragma once

#include "IInputTreeNode.h"
#include "InputTreeCase.h"
#include "JuceHeader.h"

class InputTreeRootNode : public IInputTreeNode {
  public:
    InputTreeRootNode(const juce::XmlElement& source);
    NoteContext& visit(NoteContext& context) override;

  private:
    std::vector<std::unique_ptr<IInputTreeNode>> children;
};