#pragma once

#include "JuceHeader.h"
#include "BufferedNote.h"
#include "Configuration.h"

class NoteProcessor {
public:
	NoteProcessor(const NoteContext& note, Configuration* configuration, const std::unordered_set<std::string>& tags, int channel);

	void applyStartDelay();
	int getStartDelaySamples();
	int getEndDelaySamples();

	std::vector<juce::MidiMessage> getResults();
private:
	double sampleRate;
	int channel;

	int startDelaySamples = 0;
	int endDelaySamples = 0;

	BufferedNote* target;
	std::vector<juce::MidiMessage> beforeNoteMessages;
	std::vector<juce::MidiMessage> afterNoteMessages;

	void addStartDelay(double delay);
	void addEndDelay(double delay);

	void addBeforeNote(juce::MidiMessage message);
	void addAfterNote(juce::MidiMessage message);
};