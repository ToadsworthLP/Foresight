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
    addChildComponent(tabbedComponent);
    tabbedComponent.setVisible(true);

    mainComponent = std::make_unique<GuiMainComponent>();
    editorComponent = std::make_unique<GuiEditorComponent>();

    tabbedComponent.addTab("Home", getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), &*mainComponent, false, 0);
    tabbedComponent.addTab("Configure", getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), &*editorComponent, false, 1);

    editorComponent->setConfigUpdatedCallback([this](std::string config) {
        applyButtonClicked(config);
    });

    audioProcessor.setUpdateGuiCallback([this](const std::string configName, const std::string configLatency, const std::string configSource) {
        updateGui(configName, configLatency, configSource);
    });

    audioProcessor.updateGui();

    setResizable(true, true);
    setResizeLimits(300, 300, INT_MAX, INT_MAX);

    setSize(audioProcessor.currentWindowWidth, audioProcessor.currentWindowHeight);
}

ForesightAudioProcessorEditor::~ForesightAudioProcessorEditor()
{
    audioProcessor.currentWindowWidth = getWidth();
    audioProcessor.currentWindowHeight = getHeight();
}

//==============================================================================
void ForesightAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void ForesightAudioProcessorEditor::resized()
{
    tabbedComponent.setBoundsRelative(0, 0, 1, 1);
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
