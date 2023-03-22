#pragma once

#include "JuceHeader.h"
#include "IInputTreeNode.h"
#include "InputTreeCase.h"

class InputTreeSwitchNode : public IInputTreeNode {
public:
	explicit InputTreeSwitchNode(const juce::XmlElement& source);
	NoteContext& visit(NoteContext& context) override;
private:
	enum TargetType { CC, VELOCITY, LEGATO, LENGTH, NOTE, KEYSWITCH, PROGRAM };

	std::vector<std::tuple<InputTreeCase, std::vector<std::unique_ptr<IInputTreeNode>>>> children;
	int targetNumber;
	TargetType target;

	int getTargetValue(NoteContext& context);
};