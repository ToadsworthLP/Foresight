cmake_minimum_required(VERSION 3.22)

# read VERSION file for version
file(STRINGS VERSION CURRENT_VERSION)

set(CMAKE_OSX_DEPLOYMENT_TARGET "11" CACHE STRING "Support macOS down to Big Sur")

project(Foresight VERSION ${CURRENT_VERSION})

# By default we don't want Xcode schemes to be made for modules, etc
set(CMAKE_XCODE_GENERATE_SCHEME OFF)

add_subdirectory(deps)

juce_add_plugin(Foresight
    COMPANY_NAME ToadsworthLP
    COMPANY_WEBSITE "toadsworthlp.github.io"
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    VST3_CATEGORIES Delay
    PLUGIN_MANUFACTURER_CODE Twlp
    PLUGIN_CODE Fsgt
    FORMATS VST3 AU AUv3
    PRODUCT_NAME "Foresight"
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR ${CMAKE_CURRENT_LIST_DIR}/output
)

juce_generate_juce_header(Foresight)

target_sources(Foresight
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

target_compile_features(Foresight PRIVATE cxx_std_20)

target_compile_definitions(Foresight
    PRIVATE
        CURRENT_VERSION="${CURRENT_VERSION}"
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Foresight
    PRIVATE
        juce::juce_audio_plugin_client
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
