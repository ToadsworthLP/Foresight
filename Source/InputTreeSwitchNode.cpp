#include "InputTreeSwitchNode.h"
#include "InputTreeNodeFactory.h"

InputTreeSwitchNode::InputTreeSwitchNode(const juce::XmlElement& source)
{
    if (!source.hasAttribute("target")) {
        throw std::exception("Encountered a <switch> node without the required target attribute.");
    }

    std::string targetStr = source.getStringAttribute("target").toStdString();
    if (targetStr == "legato") target = LEGATO;
    else if (targetStr == "velocity") target = VELOCITY;
    else if (targetStr == "length") target = LENGTH;
    else {
        if (targetStr.starts_with("CC")) {
            std::string trimmed = targetStr.substr(2, targetStr.length());
            ccNumber = std::stoi(trimmed);

            target = CC;
        }
        else {
            throw std::exception("Encountered a <switch> node target attribute with an invalid value.");
        }
    }

    /*for (const auto& childElement : source.getChildIterator()) {
        IInputTreeNode* child = InputTreeNodeFactory::make(*childElement);
        children.emplace_back(std::make_tuple(InputTreeCase(*childElement), child));
    }*/

    for (const auto& caseEntryElement : source.getChildIterator()) {
        if (caseEntryElement->getTagName() == "case") {
            int insertIndex = children.size();
            children.emplace_back(std::make_tuple(InputTreeCase(*caseEntryElement), std::vector<std::unique_ptr<IInputTreeNode>>()));

            for (const auto& caseChildElement : caseEntryElement->getChildIterator()) {
                IInputTreeNode* child = InputTreeNodeFactory::make(*caseChildElement);
                std::get<1>(children[insertIndex]).emplace_back(child);
            }
        }
        else {
            int insertIndex = children.size();
            children.emplace_back(std::make_tuple(InputTreeCase(*caseEntryElement), std::vector<std::unique_ptr<IInputTreeNode>>()));

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
            return context.isLegato();
            break;
        case LENGTH:
            return context.getLength().has_value() ? context.getLength().value() % INT_MAX : INT_MAX;
            break;
        case CC:
            return context.getCCValue(ccNumber);
            break;
    }
}
