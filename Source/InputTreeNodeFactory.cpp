#include "InputTreeNodeFactory.h"
#include "InputTreeSwitchNode.h"
#include "InputTreeTagNode.h"

IInputTreeNode* InputTreeNodeFactory::make(const juce::XmlElement& source)
{
    IInputTreeNode* node = nullptr;

    if (source.getTagName() == "switch") {
        node = new InputTreeSwitchNode(source);
    }
    else if (source.getTagName() == "case") {
        throw std::runtime_error("Encountered a <case> node that is not a direct child of a <switch> node.");
    }
    else {
        node = new InputTreeTagNode(source);
    }

    return node;
}
