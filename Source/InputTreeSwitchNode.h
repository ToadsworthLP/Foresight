#pragma once

#include "IInputTreeNode.h"
#include "InputTreeCase.h"
#include "JuceHeader.h"

class InputTreeSwitchNode : public IInputTreeNode {
  public:
    InputTreeSwitchNode(const juce::XmlElement& source);
    NoteContext& visit(NoteContext& context) override;

  private:
    enum TargetType { CC,
                      VELOCITY,
                      LEGATO,
                      LENGTH,
                      NOTE,
                      PROGRAM };

    std::vector<std::tuple<InputTreeCase, std::vector<std::unique_ptr<IInputTreeNode>>>> children;
    int targetNumber;
    TargetType target;

    int getTargetValue(NoteContext& context);
};