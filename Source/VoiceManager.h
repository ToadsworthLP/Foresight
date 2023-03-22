#pragma once

#include "JuceHeader.h"
#include "Configuration.h"

class VoiceManager {
public:
	VoiceManager() = default;
	~VoiceManager() = default;
	/// <summary>
	/// Returns a new MidiBuffer containing the events from the input buffer,
	/// distributed across MIDI channels so that each channel is strictly monophonic.
	/// </summary>
	/// <param name="buffer">The input MidiBuffer</param>
	/// <returns>The processed buffer</returns>
	juce::MidiBuffer processBuffer(const juce::MidiBuffer& buffer);

	/// <summary>
	/// Gets the amount of currently playing voices after processing the last buffer.
	/// </summary>
	/// <returns>The amount of playing voices</returns>
	int getCurrentVoiceCount() const;

	/// <summary>
	/// Resets the internal state of the VoiceManager. This should be called when playback is stopped.
	/// </summary>
	void reset();

	void updateConfiguration(Configuration* c);
private:
	std::optional<int> heldNote;
	Configuration* configuration;
};