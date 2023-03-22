#pragma once

#include <JuceHeader.h>
#include "Versioning.h"
#include "IInputTreeNode.h"
#include "OutputListNode.h"

class Configuration {
public:
	Configuration();
	explicit Configuration(const std::string& xml);
	std::string getSourceXML() const;
	std::string getName() const;
	double getLatencySeconds() const;
	double getSampleRate() const;
	void updateSampleRate(double sampleRate);
	int getLatencySamples() const;
	bool isInRange(int noteNumber) const;
	bool isKeyswitch(int noteNumber) const;
	bool isBlocked(const juce::MidiMessage& message) const;

	std::unordered_set<std::string> getTagsForNote(NoteContext& context) const;
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