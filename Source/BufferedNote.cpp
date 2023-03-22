#include "BufferedNote.h"

BufferedNote::BufferedNote(const juce::MidiMessage& noteOnMessage, unsigned long long startTime)
{
	this->pitch = noteOnMessage.getNoteNumber();
	this->velocity = noteOnMessage.getVelocity();
	this->startTime = startTime;
	this->endTime = std::optional<unsigned long long>();
}

std::optional<unsigned long long> BufferedNote::length()
{
	if (endTime.has_value()) {
		return startTime - endTime.value();
	}
	else {
		return std::optional<int>();
	}
}

bool BufferedNote::hasEnd() const
{
	return endTime.has_value();
}