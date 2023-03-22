#include "VoiceProcessor.h"
#include "PluginProcessor.h"
#include "NoteContext.h"
#include "NoteProcessor.h"

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

juce::MidiBuffer VoiceProcessor::processBuffer(const juce::MidiBuffer& buffer, int channel, int lengthSamples, bool bypassed)
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
			output = processSample(std::optional<std::vector<juce::MidiMessage>>(eventsPerSample[i]), channel, bypassed);
		}
		else {
			output = processSample(std::optional<std::vector<juce::MidiMessage>>(), channel, bypassed);
		}

		for (const auto& message : output) {
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

void VoiceProcessor::updateConfiguration(Configuration* c)
{
	reset();
	this->bufferSizeSamples = c->getLatencySamples();
	this->configuration = c;
}

unsigned long long VoiceProcessor::getReadPosition() const
{
	return readHeadPosition;
}

unsigned long long VoiceProcessor::getWritePosition() const
{
	return readHeadPosition + bufferSizeSamples;
}

std::vector<juce::MidiMessage> VoiceProcessor::processSample(const std::optional<std::vector<juce::MidiMessage>>& enteredMessages, int channel, bool bypassed)
{
	std::vector<juce::MidiMessage> output;

	// Read from the buffer

	// Controller data, bypassed messages or notes outside range
	while (!unprocessedBuffer.empty() && unprocessedBuffer.front().time <= getReadPosition()) {
		juce::MidiMessage message = unprocessedBuffer.front().message;
		if (message.isController()) readPositionCCStates[message.getControllerNumber()] = message.getControllerValue();
		if (message.isNoteOn()) readPositionHeldNotes[message.getNoteNumber()] = message.getVelocity();
		if (message.isNoteOff()) readPositionHeldNotes[message.getNoteNumber()] = 0;
		if (message.isProgramChange()) readPositionProgram = message.getProgramChangeNumber();

		if(!configuration->isBlocked(message)) output.emplace_back(unprocessedBuffer.front().message);
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
			NoteContext context = NoteContext(note, previousNoteAtReadPosition, readPositionCCStates, readPositionHeldNotes, readPositionProgram, lastKeyswitch);
			const std::unordered_set<std::string> tags = configuration->getTagsForNote(context);
			NoteProcessor noteProcessor = NoteProcessor(context, configuration, tags, channel);
			const std::vector<juce::MidiMessage> results = noteProcessor.getResults();

#if DEBUG
			DBG("Read Note " << note->pitch << ": ");
			for (const auto& tag : tags) {
				DBG(tag << " ");
			}
#endif

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
			if (bypassed) {
				unprocessedBuffer.emplace_back(message, getWritePosition());
			} else if (!message.isNoteOnOrOff() || (message.isNoteOnOrOff() && !configuration->isInRange(message.getNoteNumber()))) { // Controller data or notes outside range
				unprocessedBuffer.emplace_back(message, getWritePosition());
				if (message.isController()) writePositionCCStates[message.getControllerNumber()] = message.getControllerValue();
				if (message.isNoteOn()) writePositionHeldNotes[message.getNoteNumber()] = message.getVelocity();
				if (message.isNoteOff()) writePositionHeldNotes[message.getNoteNumber()] = -1;
				if (message.isProgramChange()) writePositionProgram = message.getProgramChangeNumber();
				if (configuration->isKeyswitch(message.getNoteNumber())) {
					lastKeyswitch = message.getNoteNumber();

#ifdef DEBUG
					if (message.isNoteOn()) {
						DBG("Keyswitch: " << lastKeyswitch);
					}
#endif
				}
			}
			else if (message.isNoteOff() && heldNoteAtWritePosition != nullptr) { // Note off
				heldNoteAtWritePosition->endTime = getWritePosition();
				lastWrittenNote = *heldNoteAtWritePosition;
				previousNoteAtWritePosition = *heldNoteAtWritePosition;
				heldNoteAtWritePosition = nullptr;
			}
			else if (message.isNoteOn()) { // Note on
				auto newNote = new BufferedNote(message, getWritePosition());
				bufferedNotes.emplace_back(newNote);
				heldNoteAtWritePosition = newNote;

				// Process new note - only start delay is processed here
				NoteContext context = NoteContext(newNote, previousNoteAtWritePosition, writePositionCCStates, writePositionHeldNotes, writePositionProgram, lastKeyswitch);
				const std::unordered_set<std::string> tags = configuration->getTagsForNote(context);
				NoteProcessor noteProcessor = NoteProcessor(context, configuration, tags, channel);
				noteProcessor.applyStartDelay();

				// Re-time events that should probably happen at the same time as the note
   				if (std::abs(noteProcessor.getStartDelaySamples()) > 0) {
					for (auto& unprocessedMsg : unprocessedBuffer) { // ugh
						// 512 samples or about 10ms at 48000kHz is a very generous window, so it's probably even somewhat playable
						if (getWritePosition() - unprocessedMsg.time < 512) {
							unprocessedMsg.time = newNote->startTime;
						}
					}
				}
#if DEBUG
				DBG("Write Note " << newNote->pitch << ": ");
				for (const auto& tag : tags) {
					DBG(tag << " ");
				}
#endif
			}
		}
	}

	return output;
}
