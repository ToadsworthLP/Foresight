/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ForesightAudioProcessor::ForesightAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    setLatencySamples(0);
    voiceManager = std::make_unique<VoiceManager>();
    voiceProcessors.reserve(16);

#if DEBUG
    debugFile.open("E:/MainDebug.log", std::ios::out | std::ios::app);
#endif
}

ForesightAudioProcessor::~ForesightAudioProcessor()
{
#if DEBUG
    debugFile.close();
#endif
}

//==============================================================================
const juce::String ForesightAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ForesightAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ForesightAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ForesightAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ForesightAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ForesightAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ForesightAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ForesightAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ForesightAudioProcessor::getProgramName (int index)
{
    return {};
}

void ForesightAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ForesightAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastPlayingState = false;

    setLatencySamples(sampleRate);

    for (size_t i = 0; i < 16; i++)
    {
        voiceProcessors.emplace_back(sampleRate);
    }
}

void ForesightAudioProcessor::releaseResources()
{
    lastPlayingState = false;

    voiceManager->reset();

    voiceProcessors.clear();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ForesightAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ForesightAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& inputMidi)
{
    auto playheadInfo = getPlayHead()->getPosition();
    bool isPlaying = playheadInfo->getIsPlaying();

#if DEBUG
    if (isPlaying && !lastPlayingState) debugFile << "Playback started" << std::endl;

    if (!isPlaying && lastPlayingState) debugFile << "Playback stopped" << std::endl;
#endif

    buffer.clear();

    // Clear everything if it was just stopped or started
    if (!isPlaying && lastPlayingState) {
        clearState();

        juce::MidiBuffer noteStopBuffer;
        for (int i = 0; i < 16; i++)
        {
            noteStopBuffer.addEvent(juce::MidiMessage::allNotesOff(i + 1), 0);
        }

        inputMidi.swapWith(noteStopBuffer); // Also send note off if it was stopped

#if DEBUG
        debugFile << "Note off sent on reset" << std::endl;
#endif
    }
    else if (isPlaying && !lastPlayingState) {
        clearState();
    } 
    
    if (isPlaying)
    {
        // Split the input into a maximum of 16 monophonic voices
        juce::MidiBuffer splitBuffer = voiceManager->processBuffer(inputMidi);

        // Process each voice seperately
        juce::MidiBuffer channelBuffers[16];
        for (int i = 0; i < 16; i++)
        {
            channelBuffers[i] = voiceProcessors[i].processBuffer(splitBuffer, i + 1, buffer.getNumSamples());
        }

        // Merge the buffers
        juce::MidiBuffer mergedOutputBuffer;
        for (const juce::MidiBuffer& channelBuffer : channelBuffers)
        {
            mergedOutputBuffer.addEvents(channelBuffer, 0, -1, 0);
        }

        inputMidi.swapWith(mergedOutputBuffer);
    }
    // Bypass all processing if the host is not playing

    lastPlayingState = isPlaying;
}

//==============================================================================
bool ForesightAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ForesightAudioProcessor::createEditor()
{
    return new ForesightAudioProcessorEditor (*this);
}

//==============================================================================
void ForesightAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ForesightAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void ForesightAudioProcessor::clearState()
{
    voiceManager->reset();
    for (auto& voiceProcessor : voiceProcessors)
    {
        voiceProcessor.reset();
    }

#if DEBUG
    debugFile << "Reset! " << std::endl;
#endif
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ForesightAudioProcessor();
}
