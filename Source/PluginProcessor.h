/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "VoiceProcessor.h"
#include "VoiceManager.h"
#include "Versioning.h"
#include "Configuration.h"

#define OUTPUT_CHANNEL_COUNT 16

//==============================================================================
/**
*/
class ForesightAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    ForesightAudioProcessor();
    ~ForesightAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    bool setConfiguration(const std::string& configurationXml);
    void setDefaultConfiguration();
    Configuration& getConfiguration();

    void updateGui();
    void setUpdateGuiCallback(const std::function<void(const std::string&, const  std::string&, const std::string&)>& callback);

private:
    double lastSampleRate = 0;

    std::unique_ptr<Configuration> configuration;
    std::unique_ptr<VoiceManager> voiceManager;
    std::vector<VoiceProcessor> voiceProcessors;

    bool lastPlayingState = false;
    void clearState();

    bool isCreatingEditor = false;
    std::function<void(const std::string&, const  std::string&, const std::string&)> updateGuiCallback;

    std::atomic_bool isCurrentlyInsideProcessBlock = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ForesightAudioProcessor)
};
