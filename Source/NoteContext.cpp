#include "NoteContext.h"

NoteContext::NoteContext(BufferedNote* note, const std::optional<BufferedNote>& previousNote, int ccStates[128])
{
	this->note = note;
	this->previousNote = previousNote;
	this->ccStates = ccStates;
}

int NoteContext::getVelocity()
{
	return note->velocity;
}

int NoteContext::getCCValue(const int number)
{
	return ccStates[number];
}

bool NoteContext::isLegato()
{
	return previousNote.has_value() && llabs(note->startTime - previousNote->endTime.value()) <= 16;
}

std::optional<unsigned long long> NoteContext::getLength()
{
	return note->endTime.has_value() ? note->endTime.value() - note->startTime : std::optional<unsigned long long>();
}

void NoteContext::addTag(const std::string& name)
{
	tags.insert(name);
}

std::unordered_set<std::string> NoteContext::getTags()
{
	return tags;
}

