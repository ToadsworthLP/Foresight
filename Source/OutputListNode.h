#pragma once

#include "BufferedNote.h"
#include "JuceHeader.h"
#include "NoteContext.h"

class OutputListNode {
  public:
    OutputListNode();
    OutputListNode(const juce::XmlElement& source);

    enum TargetType { CC,
                      NOTE,
                      START,
                      END,
                      LEGATO,
                      PROGRAM };
    TargetType getTargetType();
    int getCCNumber();
    int getValue(const NoteContext& context);

  private:
    TargetType target;
    int ccNumber = 0;
    int value = 0;
    std::optional<int> readCC;
    std::optional<float> readValueMultiplier;
};