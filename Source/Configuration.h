#pragma once

#include <JuceHeader.h>
#include "Versioning.h"
#include "IInputTreeNode.h"

class Configuration {
public:
	Configuration();
	Configuration(const std::string& xml);
	std::string getSourceXML();
	std::string getName();
	double getLatencySeconds();
	void updateSampleRate(double sampleRate);
	int getLatencySamples();

	NoteContext& processNote(NoteContext& context);
private:
	int version;
	double lastSampleRate;
	double latency;
	std::string name;
	std::string xml;

	std::unique_ptr<IInputTreeNode> inputTreeRoot;
};