#pragma once

#include "JuceHeader.h"

class BufferedMidiMessage {
  public:
    BufferedMidiMessage(const juce::MidiMessage& message, unsigned long long time) :
        message(message), time(time) {}
    ~BufferedMidiMessage() {}

    juce::MidiMessage message;
    unsigned long long time;
};