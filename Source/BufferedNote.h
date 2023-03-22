#pragma once

#include "JuceHeader.h"

class BufferedNote {
public:
	BufferedNote(const juce::MidiMessage& noteOnMessage, unsigned long long startTime);
	BufferedNote(int pitch, int velocity, unsigned long long startTime, unsigned long long endTime) : pitch(pitch), velocity(velocity), startTime(startTime), endTime(endTime) {}
	BufferedNote(int pitch, int velocity, unsigned long long startTime) : pitch(pitch), velocity(velocity), startTime(startTime), endTime(std::optional<unsigned long long>()) {}
	~BufferedNote() = default;

	std::optional<unsigned long long> length();
	bool hasEnd() const;

	int pitch;
	int velocity;
	unsigned long long startTime;
	int endDelay = 0;
	std::optional<unsigned long long> endTime;
	bool allowLegato = false;
};