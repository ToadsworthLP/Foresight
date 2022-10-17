#include "NoteContext.h"

NoteContext::NoteContext(BufferedNote* note, const std::optional<BufferedNote>& previousNote, int ccStates[128], int heldNotes[128], int program)
{
	this->note = note;
	this->previousNote = previousNote;
	this->ccStates = ccStates;
	this->heldNotes = heldNotes;
	this->program = program;
}

int NoteContext::getVelocity() const
{
	return note->velocity;
}

int NoteContext::getCCValue(const int number) const
{
	return ccStates[number];
}

int NoteContext::getActiveProgram() const
{
	return program;
}

int NoteContext::getHeldNoteVelocity(const int number) const
{
	return heldNotes[number];
}

bool NoteContext::isLegato() const
{
	// 64 samples should definitely be enough to catch a legato even on inaccurate DAWs without creating unintentional legato transitions
	return previousNote.has_value() && previousNote->allowLegato && llabs(note->startTime - previousNote->endTime.value()) <= 64;
}

std::optional<unsigned long long> NoteContext::getLength() const
{
	return note->endTime.has_value() ? note->endTime.value() - note->startTime : std::optional<unsigned long long>();
}

void NoteContext::addTag(const std::string& name)
{
	tags.insert(name);
}

std::unordered_set<std::string> NoteContext::getTags() const
{
	return tags;
}

BufferedNote* NoteContext::getNote() const
{
	return note;
}

