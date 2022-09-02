#include "VoiceManager.h"

VoiceManager::VoiceManager()
{
	
}

VoiceManager::~VoiceManager()
{
}

juce::MidiBuffer VoiceManager::processBuffer(const juce::MidiBuffer& buffer)
{
	juce::MidiBuffer processedBuffer;

	// Group events by time
	std::map<int, std::vector<juce::MidiMessage>> eventsPerSample;
	for (const auto metadata : buffer) {
		auto message = metadata.getMessage();
		const auto time = metadata.samplePosition;

		if (!eventsPerSample.contains(time)) {
			eventsPerSample[time] = std::vector<juce::MidiMessage>();
		}

		eventsPerSample[time].push_back(message);
	}

	// Process voices
	// Just makes it strictly monophonic for now
	for (const auto entry : eventsPerSample) { // For each group...
		int time = entry.first;

		// ...copy over CC changes
		for (auto message : entry.second)
		{
			if (!message.isNoteOnOrOff()) {
				message.setChannel(1);
				processedBuffer.addEvent(message, time);
			}
		}

		// ...look for a note-off first
		for (auto message : entry.second)
		{
			if (message.isNoteOff() && heldNote.has_value() && heldNote == message.getNoteNumber()) {
				heldNote.reset();
				currentVoiceCount--;

				message.setChannel(1);
				processedBuffer.addEvent(message, time);

				break;
			}
		}

		if (heldNote.has_value()) continue; // ...ignore everything if a note is still currently held...

		// ...or queue the next-best note-on if not
		for (auto message : entry.second)
		{
			if (message.isNoteOn()) {
				heldNote = message.getNoteNumber();
				currentVoiceCount++;

				message.setChannel(1);
				processedBuffer.addEvent(message, time);

				break; // Discard the rest
			}
		}
	}

	return processedBuffer;
}

int VoiceManager::getCurrentVoiceCount()
{
	return currentVoiceCount;
}

void VoiceManager::reset()
{
	heldNote.reset();
	currentVoiceCount = 0;
}
