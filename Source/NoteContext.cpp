#include "NoteContext.h"

NoteContext::NoteContext(BufferedNote* note, const std::optional<BufferedNote>& previousNote, std::array<int, 128> ccStates, std::array<int, 128> heldNotes, int program, int lastKeyswitch)
{
	this->note = note;
	this->previousNote = previousNote;
	this->ccStates = ccStates;
	this->heldNotes = heldNotes;
	this->program = program;
	this->lastKeyswitch = lastKeyswitch;
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

int NoteContext::getLastKeyswitch() const
{
	return lastKeyswitch;
}

int NoteContext::getHeldNoteVelocity(const int number) const
{
	return heldNotes[number];
}

bool NoteContext::isLegato() const
{
	// if the previous or current note was or is a keyswitch, this isn't a legato transition
	if (note->pitch == lastKeyswitch || (previousNote.has_value() && previousNote->pitch == lastKeyswitch)) {
		return false;
	}

	// 64 samples should definitely be enough to catch a legato even on inaccurate DAWs without creating unintentional legato transitions
	return previousNote.has_value() && previousNote->allowLegato && note->startTime - previousNote->endTime.value() <= 64;
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

