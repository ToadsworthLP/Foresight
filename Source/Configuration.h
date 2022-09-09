#pragma once

#include <JuceHeader.h>
#include "Versioning.h"
#include "IInputTreeNode.h"
#include "OutputListNode.h"

class Configuration {
public:
	Configuration();
	Configuration(const std::string& xml);
	std::string getSourceXML();
	std::string getName();
	double getLatencySeconds();
	double getSampleRate();
	void updateSampleRate(double sampleRate);
	int getLatencySamples();

	std::unordered_set<std::string> getTagsForNote(NoteContext& context);
	std::optional<OutputListNode> getOutputNode(const std::string& tag);
private:
	int version;
	double lastSampleRate;
	double latency;
	std::string name;
	std::string xml;

	std::unique_ptr<IInputTreeNode> inputTreeRoot;
	std::map<std::string, OutputListNode> outputList;
};