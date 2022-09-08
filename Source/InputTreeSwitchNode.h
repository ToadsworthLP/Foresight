#pragma once

#include "JuceHeader.h"
#include "IInputTreeNode.h"
#include "InputTreeCase.h"

class InputTreeSwitchNode : public IInputTreeNode {
public:
	InputTreeSwitchNode(const juce::XmlElement& source);
	NoteContext& visit(NoteContext& context) override;
private:
	enum TargetType { CC, VELOCITY, LEGATO, LENGTH };

	std::vector<std::tuple<InputTreeCase, std::vector<std::unique_ptr<IInputTreeNode>>>> children;
	int ccNumber;
	TargetType target;

	int getTargetValue(NoteContext& context);
};