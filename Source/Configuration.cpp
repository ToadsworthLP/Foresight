#include "Configuration.h"
#include "InputTreeRootNode.h"
#include "ConfigParserUtil.h"

Configuration::Configuration() : Configuration("<foresight version=\"4\" name=\"None\">\n    <settings>\n        <latency>0</latency>\n    </settings>\n    <input></input>\n    <output></output>\n</foresight>")
{
}

Configuration::Configuration(const std::string& xml)
{
	this->xml = xml;

	std::unique_ptr<juce::XmlDocument> xmlDocument = std::make_unique<juce::XmlDocument>(xml);
	std::unique_ptr<juce::XmlElement> rootElement = xmlDocument->getDocumentElementIfTagMatches("foresight");

	// Header

	if (!rootElement) throw std::runtime_error(xmlDocument->getLastParseError().toStdString().c_str());

	version = rootElement->getIntAttribute("version", 0);
	name = rootElement->getStringAttribute("name").toStdString();

	if (version > CURRENT_CONFIG_VERSION) throw std::runtime_error("This configuration was created for a newer version of Foresight. Please update the plugin to use this configuration.");

	// Settings
	juce::XmlElement* settingsRootElement = rootElement->getChildByName("settings");

	if (settingsRootElement) {
		// Latency
		juce::XmlElement* latencySettingElement = settingsRootElement->getChildByName("latency");
		if (latencySettingElement) latency = std::stod(latencySettingElement->getAllSubText().toStdString()) / 1000.0;

		// Range
		for (const auto& rangeElement : settingsRootElement->getChildWithTagNameIterator("range")) {
			std::string rangeModeText = rangeElement->getStringAttribute("boundary", "lower").toStdString();
			int* targetVariable = rangeModeText == "upper" ? &rangeUpperBoundary : &rangeLowerBoundary;
			int noteNumber = ConfigParserUtil::keyNameToNumber(rangeElement->getAllSubText());
			*targetVariable = noteNumber;

#ifdef DEBUG
			DBG("range" << rangeModeText << " : " << noteNumber);
#endif
		}

		// Keyswitches
		for (const auto& keyswitchElement : settingsRootElement->getChildWithTagNameIterator("keyswitches")) {
			std::string keyswitchModeText = keyswitchElement->getStringAttribute("boundary", "lower").toStdString();
			int* targetVariable = keyswitchModeText == "upper" ? &keyswitchUpperBoundary : &keyswitchLowerBoundary;
			int noteNumber = ConfigParserUtil::keyNameToNumber(keyswitchElement->getAllSubText());
			*targetVariable = noteNumber;

#ifdef DEBUG
			DBG("keyswitches" << keyswitchModeText << " : " << noteNumber);
#endif
		}

		// Blocklist
		for (const auto& blockElement : settingsRootElement->getChildWithTagNameIterator("block")) {
			juce::String targetText = blockElement->getAllSubText();
			if (targetText.startsWith("CC")) {
				blocked.insert(targetText.trim().toStdString());
			}
			else {
				blocked.insert(std::to_string(ConfigParserUtil::keyNameToNumber(targetText)));
			}
		}
	}

	// Input

	juce::XmlElement* inputTreeRootElement = rootElement->getChildByName("input");

	if (!inputTreeRootElement) throw std::runtime_error("No <input> node found.");

	inputTreeRoot = std::make_unique<InputTreeRootNode>(*inputTreeRootElement);

	// Output

	juce::XmlElement* outputListRootElement = rootElement->getChildByName("output");

	if (!outputListRootElement) throw std::runtime_error("No <output> node found.");

	int maxOutputStartDelay = 0;
	for (const auto& tagElement : outputListRootElement->getChildIterator()) {
		std::string tagName = tagElement->getStringAttribute("name").toStdString();

		for (const auto& setElement : tagElement->getChildIterator()) {
			auto outputNode = OutputListNode(*setElement);
			if (outputNode.getTargetType() == OutputListNode::START && std::abs(outputNode.getValueRaw()) > maxOutputStartDelay) {
				maxOutputStartDelay = std::abs(outputNode.getValueRaw());
			}

			outputList[tagName].emplace_back(outputNode);
		}
	}

	// if the latency isn't set, set it to the absolute value of the
	// greatest 'start' target of the output nodes
	if (latency == 0.0 && maxOutputStartDelay != 0) {
		latency = maxOutputStartDelay / 1000.0;
	}
}

std::string Configuration::getSourceXML()
{
	return xml;
}

std::string Configuration::getName()
{
	return name;
}

double Configuration::getLatencySeconds()
{
	return latency;
}

double Configuration::getSampleRate()
{
	return lastSampleRate;
}

void Configuration::updateSampleRate(double sampleRate)
{
	lastSampleRate = sampleRate;
}

int Configuration::getLatencySamples()
{
	return (int)std::round(latency * lastSampleRate);
}

bool Configuration::isInRange(int noteNumber)
{
	return noteNumber >= rangeLowerBoundary && noteNumber <= rangeUpperBoundary;
}

bool Configuration::isKeyswitch(int noteNumber)
{
	return noteNumber >= keyswitchLowerBoundary && noteNumber <= keyswitchUpperBoundary;
}

bool Configuration::isBlocked(const juce::MidiMessage& message)
{
	if (message.isController()) {
		std::string ccString = "CC" + std::to_string(message.getControllerNumber());
		return blocked.contains(ccString);
	}
	else if (message.isNoteOnOrOff()) {
		return blocked.contains(std::to_string(message.getNoteNumber()));
	}

	return false;
}

std::unordered_set<std::string> Configuration::getTagsForNote(NoteContext& context)
{
	inputTreeRoot->visit(context);
	return context.getTags();
}

std::vector<OutputListNode> Configuration::getOutputNodes(const std::string& tag)
{
	return outputList[tag];
}
