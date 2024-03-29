cmake_minimum_required(VERSION 3.25)

project(FORESIGHT VERSION 1.2.0)

add_subdirectory(Deps)

juce_add_plugin(FORESIGHT
    COMPANY_NAME ToadsworthLP
    COMPANY_WEBSITE "toadsworthlp.github.io"
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    VST3_CATEGORIES Delay
    PLUGIN_MANUFACTURER_CODE Twlp
    PLUGIN_CODE Fsgt
    FORMATS VST3
    PRODUCT_NAME "Foresight"
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR ${CMAKE_CURRENT_LIST_DIR}/output
)

juce_generate_juce_header(FORESIGHT)

target_sources(FORESIGHT
    PRIVATE
        Source/BufferedMidiMessage.cpp
        Source/BufferedNote.cpp
        Source/ConfigParserUtil.cpp
        Source/Configuration.cpp
        Source/GuiEditorComponent.cpp
        Source/GuiMainComponent.cpp
        Source/InputTreeCase.cpp
        Source/InputTreeNodeFactory.cpp
        Source/InputTreeRootNode.cpp
        Source/InputTreeSwitchNode.cpp
        Source/InputTreeTagNode.cpp
        Source/NoteContext.cpp
        Source/NoteProcessor.cpp
        Source/OutputListNode.cpp
        Source/PluginEditor.cpp
        Source/PluginProcessor.cpp
        Source/VoiceManager.cpp
        Source/VoiceProcessor.cpp
)

target_compile_features(FORESIGHT PRIVATE cxx_std_20)

target_compile_definitions(FORESIGHT
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(FORESIGHT
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
