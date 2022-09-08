#pragma once

#include "JuceHeader.h"
#include "IInputTreeNode.h"

class InputTreeTagNode : public IInputTreeNode {
public:
	InputTreeTagNode(const juce::XmlElement& source);
	NoteContext& visit(NoteContext& context) override;
private:
	std::string tag;
};