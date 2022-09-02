#include "VoiceProcessor.h"

juce::MidiBuffer VoiceProcessor::processBuffer(const juce::MidiBuffer& buffer, int channel, int lengthSamples)
{
	juce::MidiBuffer outputBuffer;

	// Group events by samples
	std::map<int, std::vector<juce::MidiMessage>> eventsPerSample;
	for (const auto metadata : buffer) {
		auto message = metadata.getMessage();

		if (message.getChannel() != channel) continue;

		const auto time = metadata.samplePosition;

		if (!eventsPerSample.contains(time)) {
			eventsPerSample[time] = std::vector<juce::MidiMessage>();
		}

		eventsPerSample[time].push_back(message);
	}

	// Do processing for each sample in the buffer
	for (int i = 0; i < lengthSamples; i++)
	{
		std::vector<juce::MidiMessage> output;
		if (eventsPerSample.contains(i)) {
			output = processSample(std::optional<std::vector<juce::MidiMessage>>(eventsPerSample[i]), channel);
		}
		else {
			output = processSample(std::optional<std::vector<juce::MidiMessage>>(), channel);
		}

		for (const auto message : output) {
			outputBuffer.addEvent(message, i);
		}
	}

	// This just copies everything to the output
	//for (const auto metadata : buffer) {
	//	auto message = metadata.getMessage();

	//	if (message.getChannel() != channel) continue;

	//	const auto time = metadata.samplePosition;
	//	outputBuffer.addEvent(message, time);
	//}

	return outputBuffer;
}

void VoiceProcessor::reset()
{
	bufferedNotes.clear();
	unprocessedBuffer.clear();
	lastWrittenNote.reset();
	heldNoteAtWritePosition = nullptr;
	heldNoteAtReadPosition = nullptr;
	readHeadPosition = 0;
}

unsigned long long VoiceProcessor::getReadPosition()
{
	return readHeadPosition;
}

unsigned long long VoiceProcessor::getWritePosition()
{
	return readHeadPosition + bufferSizeSamples;
}

std::vector<juce::MidiMessage> VoiceProcessor::processSample(const std::optional<std::vector<juce::MidiMessage>> enteredMessages, int channel)
{
	std::vector<juce::MidiMessage> output;

	// Read

	// Everything else
	while (unprocessedBuffer.size() > 0 && unprocessedBuffer.front().time <= getReadPosition()) {
		BufferedMidiMessage* readMessage = unprocessedBuffer.data();
		readMessage->message.setChannel(channel);

		output.emplace_back(readMessage->message);
		unprocessedBuffer.erase(unprocessedBuffer.begin());
	}

	//Note Off
	if (heldNoteAtReadPosition && heldNoteAtReadPosition->hasEnd() && heldNoteAtReadPosition->endTime <= getReadPosition()) {
		juce::MidiMessage message = juce::MidiMessage::noteOff(channel, heldNoteAtReadPosition->pitch, (juce::uint8)heldNoteAtReadPosition->velocity);
		output.emplace_back(message);
		
		heldNoteAtReadPosition = nullptr;
		bufferedNotes.erase(bufferedNotes.begin());
	}

	//Note On
	if (bufferedNotes.size() > 0 && bufferedNotes.data() != heldNoteAtReadPosition && bufferedNotes.front().startTime <= getReadPosition()) {
		BufferedNote* readNote = bufferedNotes.data();

		juce::MidiMessage message = juce::MidiMessage::noteOn(channel, readNote->pitch, (juce::uint8)readNote->velocity);
		output.emplace_back(message);

		heldNoteAtReadPosition = readNote;
	}

	// Write
	if (enteredMessages.has_value()) {
		for (const auto message : enteredMessages.value()) {
			// Note On
			if (message.isNoteOn()) {
				bufferedNotes.emplace_back( message, getWritePosition());
				heldNoteAtWritePosition = &*bufferedNotes.rbegin();
			}
			// Note Off
			else if (message.isNoteOff()) {
				if (heldNoteAtWritePosition) {
					heldNoteAtWritePosition->endTime = getWritePosition();
					lastWrittenNote = *heldNoteAtWritePosition;
					heldNoteAtWritePosition = nullptr;
				}
			}
			// Everything else
			else {
				unprocessedBuffer.emplace_back(message, getWritePosition());
			}
		}
	}

	// Advance the head
	readHeadPosition++;

	return output;
}
