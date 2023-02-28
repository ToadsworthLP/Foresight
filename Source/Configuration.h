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
	bool isKeyswitch(int noteNumber);
	bool isBlocked(const juce::MidiMessage& message);

	std::unordered_set<std::string> getTagsForNote(NoteContext& context);
	std::vector<OutputListNode> getOutputNodes(const std::string& tag);
private:
	int version;
	double lastSampleRate;
	std::string name;
	std::string xml;

	double latency = 0.0;
	int rangeLowerBoundary = INT_MIN;
	int rangeUpperBoundary = INT_MAX;
	int keyswitchLowerBoundary = INT_MAX;
	int keyswitchUpperBoundary = INT_MIN;

	std::unique_ptr<IInputTreeNode> inputTreeRoot;
	std::map<std::string, std::vector<OutputListNode>> outputList;
	std::unordered_set<std::string> blocked;
};