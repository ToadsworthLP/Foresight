#pragma once

#include "JuceHeader.h"
#include "BufferedNote.h"

class NoteContext {
public:
	NoteContext(BufferedNote* note, const std::optional<BufferedNote>& previousNote, std::array<int, 128> ccStates, std::array<int, 128> heldNotes, int program, int lastKeyswitch);

	int getVelocity() const;
	int getCCValue(const int number) const;
	int getActiveProgram() const;
	int getLastKeyswitch() const;
	int getHeldNoteVelocity(const int number) const;
	bool isLegato() const;
	std::optional<unsigned long long> getLength() const;

	void addTag(const std::string& name);
	std::unordered_set<std::string> getTags() const;

	BufferedNote* getNote() const;
private:
	BufferedNote* note;
	std::optional<BufferedNote> previousNote;
	std::unordered_set<std::string> tags;
	std::array<int, 128> ccStates;
	std::array<int, 128> heldNotes;
	int program;
	int lastKeyswitch;
};