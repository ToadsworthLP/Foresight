#include "NoteProcessor.h"

NoteProcessor::NoteProcessor(const NoteContext& note, Configuration* configuration, const std::unordered_set<std::string>& tags, int channel)
{
	this->target = note.getNote();
	this->sampleRate = configuration->getSampleRate();
	this->channel = channel;
	
	for (const auto& tag : tags) {
		for (auto& node : configuration->getOutputNodes(tag)) {
			switch (node.getTargetType()) {
			case OutputListNode::CC:
				addBeforeNote(juce::MidiMessage::controllerEvent(channel, node.getCCNumber(), node.getValue(note)));
				break;
			case OutputListNode::NOTE:
				addBeforeNote(juce::MidiMessage::noteOn(channel, node.getValue(note), juce::uint8(64)));
				addAfterNote(juce::MidiMessage::noteOff(channel, node.getValue(note)));
				break;
			case OutputListNode::PROGRAM:
				addBeforeNote(juce::MidiMessage::programChange(channel, node.getValue(note)));
				break;
			case OutputListNode::START:
				addStartDelay(node.getValue(note));
				break;
			case OutputListNode::END:
				addEndDelay(node.getValue(note));
				break;
			case OutputListNode::LEGATO:
				note.getNote()->allowLegato = true;
				break;
			}
		}
	}
}

void NoteProcessor::addStartDelay(int delay)
{
	startDelaySamples += static_cast<int>((static_cast<double>(delay) / 1000.0) * sampleRate);
}

void NoteProcessor::addEndDelay(int delay)
{
	endDelaySamples += static_cast<int>((static_cast<double>(delay) / 1000.0) * sampleRate);
}

void NoteProcessor::addBeforeNote(juce::MidiMessage message)
{
	message.setChannel(channel);
	beforeNoteMessages.emplace_back(message);
}

void NoteProcessor::addAfterNote(juce::MidiMessage message)
{
	message.setChannel(channel);
	afterNoteMessages.emplace_back(message);
}

void NoteProcessor::applyStartDelay()
{
	target->startTime += startDelaySamples;
}

int NoteProcessor::getStartDelaySamples()
{
	return startDelaySamples;
}

int NoteProcessor::getEndDelaySamples()
{
	return endDelaySamples;
}

std::vector<juce::MidiMessage> NoteProcessor::getResults()
{
	target->endDelay += endDelaySamples;

	std::vector<juce::MidiMessage> results = std::vector<juce::MidiMessage>();

	for (const auto& message : beforeNoteMessages) {
		results.emplace_back(message);
	}

	results.emplace_back(juce::MidiMessage::noteOn(channel, target->pitch, (juce::uint8)target->velocity));

	for (const auto& message : afterNoteMessages) {
		results.emplace_back(message);
	}

 	return results;
}
