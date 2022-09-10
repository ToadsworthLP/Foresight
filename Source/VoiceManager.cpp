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
	for (const auto& metadata : buffer) {
		auto message = metadata.getMessage();
		const auto time = metadata.samplePosition;

		if (!eventsPerSample.contains(time)) {
			eventsPerSample[time] = std::vector<juce::MidiMessage>();
		}

		eventsPerSample[time].emplace_back(message);
	}

	// Process voices
	// Just makes it strictly monophonic for now
	for (auto& entry : eventsPerSample) { // For each group...
		int time = entry.first;

		// ...copy over everything that's not a note or that's outside the range
		for (auto& message : entry.second)
		{
			if (!message.isNoteOnOrOff() || (message.isNoteOnOrOff() && !configuration->isInRange(message.getNoteNumber()))) {
				message.setChannel(1);
				processedBuffer.addEvent(message, time);
			}
		}

		// ...look for a note-off first
		for (auto& message : entry.second)
		{
			if (message.isNoteOff() && configuration->isInRange(message.getNoteNumber()) && heldNote.has_value() && heldNote == message.getNoteNumber()) {
				heldNote.reset();

				message.setChannel(1);
				processedBuffer.addEvent(message, time);

				break;
			}
		}

		// ...handle note-on
		for (auto& message : entry.second)
		{
			if (message.isNoteOn() && configuration->isInRange(message.getNoteNumber()))
			{
				// If there's already a playing note, stop it
				if (heldNote.has_value())
				{
					processedBuffer.addEvent(juce::MidiMessage::noteOff(1, heldNote.value()), time);
					heldNote.reset();
				}

				// Play the new note
				
				heldNote = message.getNoteNumber();

				message.setChannel(1);
				processedBuffer.addEvent(message, time);

				break;
			}
		}
	}

	return processedBuffer;
}

int VoiceManager::getCurrentVoiceCount()
{
	return 1;
}

void VoiceManager::reset()
{
	heldNote.reset();
}

void VoiceManager::updateConfiguration(Configuration* configuration)
{
	this->configuration = configuration;
}
