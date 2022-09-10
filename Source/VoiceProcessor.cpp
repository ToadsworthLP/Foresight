#include "VoiceProcessor.h"
#include "PluginProcessor.h"
#include "NoteContext.h"
#include "NoteProcessor.h"

VoiceProcessor::VoiceProcessor()
{
}

VoiceProcessor::VoiceProcessor(const VoiceProcessor& other)
{
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

	return outputBuffer;
}

void VoiceProcessor::reset()
{
	for (const auto& entry : bufferedNotes) {
		delete(entry);
	}

	readHeadPosition = 0;

	bufferedNotes.clear();
	lastWrittenNote.reset();
	heldNoteAtWritePosition = nullptr;
	previousNoteAtWritePosition.reset();
	previousNoteAtReadPosition.reset();
	unprocessedBuffer.clear();
}

void VoiceProcessor::updateConfiguration(Configuration* configuration)
{
	reset();
	this->bufferSizeSamples = configuration->getLatencySamples();
	this->configuration = configuration;
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

	// Controller data or notes outside range
	while (!unprocessedBuffer.empty() && unprocessedBuffer.front().time <= getReadPosition()) {
		juce::MidiMessage message = unprocessedBuffer.front().message;
		if (message.isController()) readPositionCCStates[message.getControllerNumber()] = message.getControllerValue();
		if (message.isNoteOn()) readPositionHeldNotes[message.getNoteNumber()] = message.getVelocity();
		if (message.isNoteOff()) readPositionHeldNotes[message.getNoteNumber()] = 0;

		output.emplace_back(unprocessedBuffer.front().message);
		unprocessedBuffer.erase(unprocessedBuffer.begin());
	}

	// Note off
	for (size_t i = 0; i < bufferedNotes.size(); i++)
	{
		BufferedNote* note = bufferedNotes[i];
		if (note->endTime && note->endTime.value() + note->endDelay <= getReadPosition()) {
			output.emplace_back(juce::MidiMessage::noteOff(channel, note->pitch));
			bufferedNotes.erase(bufferedNotes.begin() + i);
			previousNoteAtReadPosition = *note;
			delete(note);
		}
	}

	// Note on
	for (const auto& note : bufferedNotes) {
		if (note->startTime == getReadPosition()) {
			// Process note that's about to play - everything but start delay is processed here
			NoteContext context = NoteContext(note, previousNoteAtReadPosition, readPositionCCStates, readPositionHeldNotes);
			std::unordered_set<std::string> tags = configuration->getTagsForNote(context);
			NoteProcessor noteProcessor = NoteProcessor(note, configuration, tags, channel);
			std::vector<juce::MidiMessage> results = noteProcessor.getResults();

			for (const auto& message : results) {
				output.emplace_back(message);
			}
		}
	}

	// Advance the head
	readHeadPosition++;

	// Write to the buffer
	if (enteredMessages.has_value()) {
		for (const auto& message : enteredMessages.value()) {
			if (!message.isNoteOnOrOff() || (message.isNoteOnOrOff() && !configuration->isInRange(message.getNoteNumber()))) { // Controller data or notes outside range
				unprocessedBuffer.emplace_back(message, getWritePosition());
				if (message.isController()) writePositionCCStates[message.getControllerNumber()] = message.getControllerValue();
				if (message.isNoteOn()) writePositionHeldNotes[message.getNoteNumber()] = message.getVelocity();
				if (message.isNoteOff()) writePositionHeldNotes[message.getNoteNumber()] = -1;
			}
			else if (message.isNoteOff() && heldNoteAtWritePosition) { // Note off
				heldNoteAtWritePosition->endTime = getWritePosition();
				lastWrittenNote = *heldNoteAtWritePosition;
				previousNoteAtWritePosition = *heldNoteAtWritePosition;
				heldNoteAtWritePosition = nullptr;
			}
			else if (message.isNoteOn()) { // Note on
				BufferedNote* newNote = new BufferedNote(message, getWritePosition());
				bufferedNotes.emplace_back(newNote);
				heldNoteAtWritePosition = newNote;

				// Process new note - only start delay is processed here
				NoteContext context = NoteContext(newNote, previousNoteAtWritePosition, writePositionCCStates, writePositionHeldNotes);
				std::unordered_set<std::string> tags = configuration->getTagsForNote(context);
				NoteProcessor noteProcessor = NoteProcessor(newNote, configuration, tags, channel);
				noteProcessor.applyStartDelay();
			}
		}
	}

	return output;
}
