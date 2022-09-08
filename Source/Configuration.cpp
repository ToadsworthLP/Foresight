#include "Configuration.h"
#include "InputTreeRootNode.h"

Configuration::Configuration() : Configuration("<foresight version=\"0\" name=\"None\">\n	<input></input>\n</foresight>")
{
}

Configuration::Configuration(const std::string& xml)
{
	this->xml = xml;

	std::unique_ptr<juce::XmlDocument> xmlDocument = std::make_unique<juce::XmlDocument>(xml);
	std::unique_ptr<juce::XmlElement> rootElement = xmlDocument->getDocumentElementIfTagMatches("foresight");

	if (!rootElement) throw std::exception(xmlDocument->getLastParseError().toStdString().c_str());

	version = rootElement->getIntAttribute("version", 0);
	name = rootElement->getStringAttribute("name").toStdString();

	if (version > CURRENT_CONFIG_VERSION) throw std::exception("This configuration was created for a newer version of Foresight. Please update the plugin to use this configuration.");

	juce::XmlElement* inputTreeRootElement = rootElement->getChildByName("input");

	if (!inputTreeRootElement) throw std::exception("No <input> node found.");

	inputTreeRoot = std::make_unique<InputTreeRootNode>(*inputTreeRootElement);
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

void Configuration::updateSampleRate(double sampleRate)
{
	lastSampleRate = sampleRate;
}

int Configuration::getLatencySamples()
{
	return (int)std::round(latency * lastSampleRate);
}

NoteContext& Configuration::processNote(NoteContext& context)
{
	return inputTreeRoot->visit(context);
}
