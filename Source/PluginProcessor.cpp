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
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    setLatencySamples(0);

    voiceManager = std::make_unique<VoiceManager>();
    voiceProcessors.reserve(16);

    if(!configuration) setDefaultConfiguration();
}

ForesightAudioProcessor::~ForesightAudioProcessor()
{
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
    lastSampleRate = sampleRate;

    configuration->updateSampleRate(lastSampleRate);
    setLatencySamples(configuration->getLatencySamples());

    for (size_t i = 0; i < 16; i++)
    {
        VoiceProcessor& processor = voiceProcessors.emplace_back();
        processor.updateConfiguration(configuration.get());
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

void ForesightAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& inputMidi)
{
    processMidi(buffer, inputMidi, false);
}

void ForesightAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& inputMidi)
{
    processMidi(buffer, inputMidi, true);
}

void ForesightAudioProcessor::processMidi(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& inputMidi, bool bypassed)
{
    isCurrentlyInsideProcessBlock = true;

    auto playheadInfo = getPlayHead()->getPosition();
    const bool isPlaying = playheadInfo->getIsPlaying();

    buffer.clear();

    // Clear everything if it was just stopped or started
    if (!isPlaying && lastPlayingState) {
        clearState();

        // Also send note off messages if it was stopped
        juce::MidiBuffer noteStopBuffer;
        for (int channel = 1; channel <= 16; channel++)
        {
            for (int note = 0; note < 128; note++)
            {
                noteStopBuffer.addEvent(juce::MidiMessage::noteOff(channel, note), 0);
            }
        }

        inputMidi.swapWith(noteStopBuffer); 
    }
    else if (isPlaying && !lastPlayingState) {
        clearState();
    }

#ifdef DEBUG
    // JUCE plugin host never reports that it is playing
    if (true)
#else
    // Bypass all processing if the host is not playing
    if (isPlaying)
#endif
    {
        // Split the input into a maximum of 16 monophonic voices
        juce::MidiBuffer splitBuffer = voiceManager->processBuffer(inputMidi);

        // Process each voice separately
        std::array<juce::MidiBuffer, 16> channelBuffers;
        for (int i = 0; i < 16; i++)
        {
            channelBuffers[i] = voiceProcessors[i].processBuffer(splitBuffer, i + 1, buffer.getNumSamples(), bypassed);
        }

        // Merge the buffers
        juce::MidiBuffer mergedOutputBuffer;
        for (const juce::MidiBuffer& channelBuffer : channelBuffers)
        {
            mergedOutputBuffer.addEvents(channelBuffer, 0, -1, 0);
        }

        inputMidi.swapWith(mergedOutputBuffer);
    }

    lastPlayingState = isPlaying;

    isCurrentlyInsideProcessBlock = false;
}

//==============================================================================
bool ForesightAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ForesightAudioProcessor::createEditor()
{
    isCreatingEditor = true;
    auto editor = new ForesightAudioProcessorEditor (*this);
    isCreatingEditor = false;

    return editor;
}

//==============================================================================
void ForesightAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> state = std::make_unique<juce::XmlElement>("foresight-state");
    state->setAttribute("version", CURRENT_STATE_VERSION);

    juce::XmlElement* sourceElement = state->createNewChildElement("source");
    sourceElement->addTextElement(configuration->getSourceXML());

    juce::XmlElement* windowSizeElement = state->createNewChildElement("window");

    auto editor = getActiveEditor();
    if (editor != nullptr) {
        currentWindowWidth = editor->getWidth();
        currentWindowHeight = editor->getHeight();
    }

    windowSizeElement->setAttribute("width", currentWindowWidth);
    windowSizeElement->setAttribute("height", currentWindowHeight);

    copyXmlToBinary(*state, destData);
}

void ForesightAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const std::unique_ptr<juce::XmlElement> state(getXmlFromBinary(data, sizeInBytes));

    // Check if the state is valid
    if (state && state->getTagName() == "foresight-state" && std::stoi(state->getAttributeValue(0).toStdString()) <= CURRENT_STATE_VERSION)
    {
        const std::string source = state->getChildByName("source")->getAllSubText().toStdString();
        setConfiguration(source);

        juce::XmlElement* windowSizeElement = state->getChildByName("window");
        if (windowSizeElement != nullptr) {
            currentWindowWidth = windowSizeElement->getIntAttribute("width");
            currentWindowHeight = windowSizeElement->getIntAttribute("height");
        }
        else {
            currentWindowWidth = 400;
            currentWindowHeight = 400;
        }
    }
    else {
        setDefaultConfiguration();
    }
}

bool ForesightAudioProcessor::setConfiguration(const std::string& configurationXml)
{
    bool success = true;

    suspendProcessing(true);

    while (isCurrentlyInsideProcessBlock) { } // Wait until the process block has finished

    try {
        configuration = std::make_unique<Configuration>(configurationXml);
        configuration->updateSampleRate(lastSampleRate);

        voiceManager->updateConfiguration(configuration.get());
        for (auto& processor : voiceProcessors) processor.updateConfiguration(configuration.get());

        setLatencySamples(configuration->getLatencySamples());
    }
    catch (const std::exception& e) {
        configuration = std::make_unique<Configuration>();
        configuration->updateSampleRate(lastSampleRate);

        voiceManager->updateConfiguration(configuration.get());
        for (auto& processor : voiceProcessors) processor.updateConfiguration(configuration.get());

        setLatencySamples(configuration->getLatencySamples());

        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Configuration Error", "Failed to load configuration: \n" + std::string(e.what()), "Ok", nullptr, nullptr);
        success = false;
    }

    suspendProcessing(false);

    updateGui();

    return success;
}

void ForesightAudioProcessor::setDefaultConfiguration()
{
    suspendProcessing(true);

    configuration = std::make_unique<Configuration>();

    suspendProcessing(false);

    updateGui();
}



Configuration& ForesightAudioProcessor::getConfiguration()
{
    return *configuration;
}

void ForesightAudioProcessor::updateGui()
{
    const bool editorAvailable = (getActiveEditor() != nullptr || isCreatingEditor);
    if (updateGuiCallback != nullptr && editorAvailable) updateGuiCallback(configuration->getName(), "Latency: " + std::to_string(configuration->getLatencySeconds()) + "s", configuration->getSourceXML());
}

void ForesightAudioProcessor::setUpdateGuiCallback(const std::function<void(const std::string&, const  std::string&, const std::string&)>& callback)
{
    updateGuiCallback = callback;
}

void ForesightAudioProcessor::clearState()
{
    voiceManager->reset();
    for (auto& voiceProcessor : voiceProcessors)
    {
        voiceProcessor.reset();
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ForesightAudioProcessor();
}
