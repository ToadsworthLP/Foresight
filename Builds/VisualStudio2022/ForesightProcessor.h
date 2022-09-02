#pragma once
#include "JuceHeader.h"

class ForesightProcessor
{
public:
	ForesightProcessor(int bufferSize) : bufferSize(bufferSize) {}
	void StartBlock(int blockSize);
	void EnterMidi(int timeOffset, juce::MidiMessage& message);
	void EndBlock();
	~ForesightProcessor();

private:
	unsigned int bufferSize = 0;
	unsigned int playheadPosition = 0;

	unsigned int currentBlockSize;
};

