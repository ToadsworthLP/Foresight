#pragma once

#include "JuceHeader.h"
#include "BufferedNote.h"

class NoteContext {
public:
	NoteContext(BufferedNote* note, const std::optional<BufferedNote>& previousNote, int ccStates[128], int heldNotes[128]);

	int getVelocity();
	int getCCValue(const int number);
	int getHeldNoteVelocity(const int number);
	bool isLegato();
	std::optional<unsigned long long> getLength();

	void addTag(const std::string& name);
	std::unordered_set<std::string> getTags();
private:
	BufferedNote* note;
	std::optional<BufferedNote> previousNote;
	std::unordered_set<std::string> tags;
	int* ccStates;
	int* heldNotes;
};