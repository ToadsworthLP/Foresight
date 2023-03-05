/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GuiMainComponent.h"
#include "GuiEditorComponent.h"

//==============================================================================
/**
*/
class ForesightAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit ForesightAudioProcessorEditor (ForesightAudioProcessor&);
    ~ForesightAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void updateGui(const std::string& configName, const std::string& configLatency, const std::string& configSource);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ForesightAudioProcessor& audioProcessor;


    juce::TabbedComponent tabbedComponent = juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtTop);
    std::unique_ptr<GuiMainComponent> mainComponent;
    std::unique_ptr<GuiEditorComponent> editorComponent;

    void applyButtonClicked(const std::string& configString);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ForesightAudioProcessorEditor)
};
