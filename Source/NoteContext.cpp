#include "NoteContext.h"

NoteContext::NoteContext(BufferedNote* note, const std::optional<BufferedNote>& previousNote, int ccStates[128], int heldNotes[128])
{
	this->note = note;
	this->previousNote = previousNote;
	this->ccStates = ccStates;
	this->heldNotes = heldNotes;
}

int NoteContext::getVelocity()
{
	return note->velocity;
}

int NoteContext::getCCValue(const int number)
{
	return ccStates[number];
}

int NoteContext::getHeldNoteVelocity(const int number)
{
	return heldNotes[number];
}

bool NoteContext::isLegato()
{
	// 64 samples should definitely be enough to catch a legato even on inaccurate DAWs without creating unintentional legato transitions
	return previousNote.has_value() && previousNote->allowLegato && llabs(note->startTime - previousNote->endTime.value()) <= 64;
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

