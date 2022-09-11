#include "OutputListNode.h"
#include "ConfigParserUtil.h"

OutputListNode::OutputListNode()
{
    target = END;
    value = 0;
}

OutputListNode::OutputListNode(const juce::XmlElement& source)
{
	std::string targetStr = source.getStringAttribute("target").toStdString();
	std::string valueStr = source.getAllSubText().toStdString();

    if (targetStr == "note") target = NOTE;
    else if (targetStr == "start") target = START;
    else if (targetStr == "end") target = END;
    else if (targetStr == "legato") target = LEGATO;
    else {
        if (targetStr.starts_with("CC")) {
            std::string trimmed = targetStr.substr(2, targetStr.length());
            ccNumber = std::stoi(trimmed);

            target = CC;
        }
        else {
            throw std::exception("Encountered a <set> node target attribute with an invalid value.");
        }
    }

    if (target == NOTE) {
        value = ConfigParserUtil::keyNameToNumber(juce::String(valueStr), 3);
    }
    else if (target != LEGATO) {
        value = std::stoi(valueStr);
    }
}

OutputListNode::TargetType OutputListNode::getTargetType()
{
    return target;
}

int OutputListNode::getCCNumber()
{
    return ccNumber;
}

int OutputListNode::getValue()
{
    return value;
}