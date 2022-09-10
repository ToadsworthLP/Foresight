/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ForesightAudioProcessorEditor::ForesightAudioProcessorEditor (ForesightAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 400);

    mainComponent = std::make_unique<GuiMainComponent>();
    editorComponent = std::make_unique<GuiEditorComponent>();

    tabbedComponent.setBounds(getBounds());
    tabbedComponent.addTab("Home", getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), &*mainComponent, false, 0);
    tabbedComponent.addTab("Configure", getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), &*editorComponent, false, 1);

    editorComponent->setConfigUpdatedCallback([this](std::string config) {
        applyButtonClicked(config);
    });

    audioProcessor.setUpdateGuiCallback([this](const std::string configName, const std::string configLatency, const std::string configSource) {
        updateGui(configName, configLatency, configSource);
    });

    audioProcessor.updateGui();
}

ForesightAudioProcessorEditor::~ForesightAudioProcessorEditor()
{
}

//==============================================================================
void ForesightAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    addAndMakeVisible(tabbedComponent);
}

void ForesightAudioProcessorEditor::resized()
{
}

void ForesightAudioProcessorEditor::updateGui(const std::string& configName, const std::string& configLatency, const std::string& configSource)
{
    if (mainComponent) {
        mainComponent->setDisplayedConfigName(configName);
        mainComponent->setDisplayedLatency(configLatency);
    }
    
    if (editorComponent) {
        editorComponent->setDisplayedConfig(configSource);
    }
}

void ForesightAudioProcessorEditor::applyButtonClicked(const std::string& configString)
{
    audioProcessor.setConfiguration(configString);
}
