#pragma once

#include "JuceHeader.h"
#include "BufferedNote.h"
#include "BufferedMidiMessage.h"
#include "Configuration.h"

class VoiceProcessor {
public:
	VoiceProcessor();
	VoiceProcessor(const VoiceProcessor& other);
	~VoiceProcessor();
	juce::MidiBuffer processBuffer(const juce::MidiBuffer& buffer, int channel, int lengthSamples, bool bypassed);
	void reset();
	void updateConfiguration(Configuration* configuration);
private:
	Configuration* configuration;

	int bufferSizeSamples;
	unsigned long long readHeadPosition = 0;

	int lastKeyswitch = -1;
	std::vector<BufferedNote*> bufferedNotes;
	std::optional<BufferedNote> lastWrittenNote;
	BufferedNote* heldNoteAtWritePosition = nullptr;
	std::optional<BufferedNote> previousNoteAtWritePosition;
	std::optional<BufferedNote> previousNoteAtReadPosition;
	int writePositionCCStates[128] = { 0 };
	int readPositionCCStates[128] = { 0 };
	int writePositionHeldNotes[128] = { 0 };
	int readPositionHeldNotes[128] = { 0 };
	int readPositionProgram = 0;
	int writePositionProgram = 0;

	std::vector<BufferedMidiMessage> unprocessedBuffer;

	unsigned long long getReadPosition();
	unsigned long long getWritePosition();

	std::vector<juce::MidiMessage> processSample(const std::optional<std::vector<juce::MidiMessage>>& enteredMessages, int channel, bool bypassed);
};