#include "Configuration.h"
#include "InputTreeRootNode.h"
#include "ConfigParserUtil.h"

Configuration::Configuration() : Configuration("<foresight version=\"0\" name=\"None\">\n    <settings>\n        <latency>0</latency>\n    </settings>\n    <input></input>\n    <output></output>\n</foresight>")
{
}

Configuration::Configuration(const std::string& xml)
{
	this->xml = xml;

	std::unique_ptr<juce::XmlDocument> xmlDocument = std::make_unique<juce::XmlDocument>(xml);
	std::unique_ptr<juce::XmlElement> rootElement = xmlDocument->getDocumentElementIfTagMatches("foresight");

	// Header

	if (!rootElement) throw std::exception(xmlDocument->getLastParseError().toStdString().c_str());

	version = rootElement->getIntAttribute("version", 0);
	name = rootElement->getStringAttribute("name").toStdString();

	if (version > CURRENT_CONFIG_VERSION) throw std::exception("This configuration was created for a newer version of Foresight. Please update the plugin to use this configuration.");

	// Settings

	juce::XmlElement* settingsRootElement = rootElement->getChildByName("settings");

	if (settingsRootElement) {
		// Latency
		juce::XmlElement* latencySettingElement = settingsRootElement->getChildByName("latency");
		if (latencySettingElement) latency = std::stod(latencySettingElement->getAllSubText().toStdString()) / 1000.0;

		// Range
		for (const auto& rangeElement : settingsRootElement->getChildWithTagNameIterator("range")) {
			std::string rangeModeText = rangeElement->getStringAttribute("mode", "above").toStdString();
			int* targetVariable = rangeModeText == "below" ? &rangeUpperBoundary : &rangeLowerBoundary;
			int noteNumber = ConfigParserUtil::keyNameToNumber(rangeElement->getAllSubText(), 3);
			*targetVariable = noteNumber;
		}
	}

	// Input

	juce::XmlElement* inputTreeRootElement = rootElement->getChildByName("input");

	if (!inputTreeRootElement) throw std::exception("No <input> node found.");

	inputTreeRoot = std::make_unique<InputTreeRootNode>(*inputTreeRootElement);

	// Output

	juce::XmlElement* outputListRootElement = rootElement->getChildByName("output");

	if (!outputListRootElement) throw std::exception("No <output> node found.");

	for (const auto& tagElement : outputListRootElement->getChildIterator()) {
		std::string tagName = tagElement->getStringAttribute("name").toStdString();

		for (const auto& setElement : tagElement->getChildIterator()) {
			outputList[tagName] = OutputListNode(*setElement);
		}
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

std::unordered_set<std::string> Configuration::getTagsForNote(NoteContext& context)
{
	inputTreeRoot->visit(context);
	return context.getTags();
}

std::optional<OutputListNode> Configuration::getOutputNode(const std::string& tag)
{
	if (!outputList.contains(tag)) return std::optional<OutputListNode>();
	return outputList[tag];
}
