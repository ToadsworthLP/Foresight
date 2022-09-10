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
	bool isInRange(int noteNumber);

	std::unordered_set<std::string> getTagsForNote(NoteContext& context);
	std::optional<OutputListNode> getOutputNode(const std::string& tag);
private:
	int version;
	double lastSampleRate;
	std::string name;
	std::string xml;

	double latency = 0.0;
	int rangeLowerBoundary = INT_MIN;
	int rangeUpperBoundary = INT_MAX;

	std::unique_ptr<IInputTreeNode> inputTreeRoot;
	std::map<std::string, OutputListNode> outputList;
};