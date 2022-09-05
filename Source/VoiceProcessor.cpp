#include "VoiceProcessor.h"
#include "PluginProcessor.h"

VoiceProcessor::VoiceProcessor(int bufferSizeSamples)
{
	this->bufferSizeSamples = bufferSizeSamples;

#if DEBUG
	debugFile.open("E:/VoiceProcDebug.log", std::ios::out | std::ios::app);
#endif
}

VoiceProcessor::VoiceProcessor(const VoiceProcessor& other)
{
#if DEBUG
	debugFile << "Copy constructor invoked!" << std::endl;
#endif

	bufferSizeSamples = other.bufferSizeSamples;
	readHeadPosition = other.readHeadPosition;

	for (const auto& note : other.bufferedNotes) {
		BufferedNote* newNote = new BufferedNote(*note);
		bufferedNotes.emplace_back(newNote);
	}

	heldNoteAtWritePosition = bufferedNotes.back();
	lastWrittenNote = other.lastWrittenNote;
	unprocessedBuffer = other.unprocessedBuffer;
}

VoiceProcessor::~VoiceProcessor()
{
#if DEBUG
	debugFile.close();
#endif
	reset();
}

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

		eventsPerSample[time].emplace_back(message);
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

	//// This just copies everything to the output
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
	for (const auto& entry : bufferedNotes) {
		delete(entry);
	}

	bufferedNotes.clear();
	unprocessedBuffer.clear();
	lastWrittenNote.reset();
	heldNoteAtWritePosition = nullptr;
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

std::vector<juce::MidiMessage> VoiceProcessor::processSample(const std::optional<std::vector<juce::MidiMessage>>& enteredMessages, int channel)
{
	std::vector<juce::MidiMessage> output;

	// Read from the buffer

	// Controller data
	while (!unprocessedBuffer.empty() && unprocessedBuffer.front().time == getReadPosition()) {
		output.emplace_back(unprocessedBuffer.front().message);
		unprocessedBuffer.erase(unprocessedBuffer.begin());
	}

	// Note off
	for (size_t i = 0; i < bufferedNotes.size(); i++)
	{
		BufferedNote* note = bufferedNotes[i];
		if (note->endTime == getReadPosition()) {
			output.emplace_back(juce::MidiMessage::noteOff(channel, note->pitch));
			bufferedNotes.erase(bufferedNotes.begin() + i);
			delete(note);
		}
	}

	// Note on
	for (const auto& note : bufferedNotes) {
		if (note->startTime == getReadPosition()) {
			output.emplace_back(juce::MidiMessage::noteOn(channel, note->pitch, (juce::uint8)note->velocity));
		}
	}

	// Write to the buffer
	if (enteredMessages.has_value()) {
		for (const auto& message : enteredMessages.value()) {
			if (!message.isNoteOnOrOff()) { // Controller data
				unprocessedBuffer.emplace_back(message, getWritePosition());
			}
			else if (message.isNoteOff()) { // Note off
				heldNoteAtWritePosition->endTime = getWritePosition();
				lastWrittenNote = *heldNoteAtWritePosition;
				heldNoteAtWritePosition = nullptr;
			}
			else if (message.isNoteOn()) { // Note on
				BufferedNote* newNote = new BufferedNote(message, getWritePosition());
				bufferedNotes.emplace_back(newNote);
				heldNoteAtWritePosition = newNote;
			}
		}
	}

	// Advance the head
	readHeadPosition++;

	return output;
}
