#include "OutputListNode.h"
#include "ConfigParserUtil.h"

OutputListNode::OutputListNode()
{
    target = END;
    value = 0;
}

OutputListNode::OutputListNode(const juce::XmlElement& source)
{
	const std::string targetStr = source.getStringAttribute("target").toStdString();
	const std::string valueStr = source.getAllSubText().toStdString();

    if (targetStr == "note") target = NOTE;
    else if (targetStr == "start") target = START;
    else if (targetStr == "end") target = END;
    else if (targetStr == "legato") target = LEGATO;
    else if (targetStr == "program") target = PROGRAM;
    else {
        if (targetStr.starts_with("CC")) {
            const std::string trimmed = targetStr.substr(2, targetStr.length());
            ccNumber = std::stoi(trimmed);

            target = CC;
        }
        else {
            throw std::runtime_error("Encountered a <set> node target attribute with an invalid value.");
        }
    }

    if (target == NOTE) {
        value = ConfigParserUtil::keyNameToNumber(juce::String(valueStr));
    }
    else if (target != LEGATO) {
        if (valueStr.starts_with("CC")) {
            auto multiplierStartIndex = valueStr.find('*');

            if (multiplierStartIndex > 0) {
                const std::string trimmed = valueStr.substr(2, valueStr.length());
                readCC = std::stoi(trimmed);

                const std::string multiplierStr = valueStr.substr(multiplierStartIndex + 1, valueStr.length());
                readValueMultiplier = std::stof(multiplierStr);
            }
            else {
                const std::string trimmed = valueStr.substr(2, valueStr.length());
                readCC = std::stoi(trimmed);
            }
        }
        else {
            value = std::stoi(valueStr);
        }
    }
}

OutputListNode::TargetType OutputListNode::getTargetType() const
{
    return target;
}

int OutputListNode::getCCNumber() const
{
    return ccNumber;
}

int OutputListNode::getValue(const NoteContext& context)
{
    if (readCC.has_value()) {
        return static_cast<int>(std::round(context.getCCValue(readCC.value()) * readValueMultiplier.value_or(1.0f)));
    }
    else {
        return value;
    }
}

int OutputListNode::getValueRaw() const
{
    return value;
}