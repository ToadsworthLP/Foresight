#include "InputTreeSwitchNode.h"
#include "InputTreeNodeFactory.h"
#include "ConfigParserUtil.h"

InputTreeSwitchNode::InputTreeSwitchNode(const juce::XmlElement& source)
{
    if (!source.hasAttribute("target")) {
        throw std::runtime_error("Encountered a <switch> node without the required target attribute.");
    }

    const std::string targetStr = source.getStringAttribute("target").toStdString();
    if (targetStr == "legato") target = LEGATO;
    else if (targetStr == "velocity") target = VELOCITY;
    else if (targetStr == "length") target = LENGTH;
    else if (targetStr == "program") target = PROGRAM;
    else {
        if (targetStr.starts_with("CC")) {
            const std::string trimmed = targetStr.substr(2, targetStr.length());
            targetNumber = std::stoi(trimmed);

            target = CC;
        }           
        else if (targetStr.starts_with("KS_")) {
            const std::string trimmed = targetStr.substr(3, targetStr.length());
            try {
                targetNumber = ConfigParserUtil::keyNameToNumber(trimmed);
                target = KEYSWITCH;
            }
            catch (...) {
                throw std::runtime_error("Encountered a <switch> node target attribute with an invalid value.");
            }
        }
        else {
            try {
                targetNumber = ConfigParserUtil::keyNameToNumber(targetStr);
                target = NOTE;
            }
            catch (...) {
                throw std::runtime_error("Encountered a <switch> node target attribute with an invalid value.");
            }
        }
    }

    int keyswitch = -1;
    if (target == KEYSWITCH) {
        keyswitch = targetNumber;
    }

    for (const auto& caseEntryElement : source.getChildIterator()) {
        if (caseEntryElement->getTagName() == "case") {
            auto insertIndex = children.size();
            children.emplace_back(std::make_tuple(InputTreeCase(*caseEntryElement, keyswitch), std::vector<std::unique_ptr<IInputTreeNode>>()));

            for (const auto& caseChildElement : caseEntryElement->getChildIterator()) {
                IInputTreeNode* child = InputTreeNodeFactory::make(*caseChildElement);
                std::get<1>(children[insertIndex]).emplace_back(child);
            }
        }
        else {
            auto insertIndex = children.size();
            children.emplace_back(std::make_tuple(InputTreeCase(*caseEntryElement, keyswitch), std::vector<std::unique_ptr<IInputTreeNode>>()));

            IInputTreeNode* child = InputTreeNodeFactory::make(*caseEntryElement);
            std::get<1>(children[insertIndex]).emplace_back(child);
        }
    }
}

NoteContext& InputTreeSwitchNode::visit(NoteContext& context)
{
    for (const auto& cases : children) {
        InputTreeCase condition = std::get<0>(cases);

        if (condition.check(getTargetValue(context))) {
            for (const auto& caseChild : std::get<1>(cases)) {
                caseChild->visit(context);
            }
        }
    }

    return context;
}

int InputTreeSwitchNode::getTargetValue(NoteContext& context)
{
    switch (target)
    {
        case VELOCITY:
            return context.getVelocity();
            break;
        case LEGATO:
            return static_cast<int>(context.isLegato());
            break;
        case LENGTH:
            if (!context.getLength().has_value()) {
                return INT_MAX;
            }
            return static_cast<int>(context.getLength().value()) % INT_MAX;
            break;
        case CC:
            return context.getCCValue(targetNumber);
            break;
        case NOTE:
            return context.getHeldNoteVelocity(targetNumber);
            break;
        case KEYSWITCH:
            return context.getLastKeyswitch();
            break;
        case PROGRAM:
            return context.getActiveProgram();
            break;
        default:
            throw std::runtime_error("Invalid <input> target value");
    }
}
