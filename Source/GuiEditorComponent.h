/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.2

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class GuiEditorComponent  : public juce::Component,
                            public juce::Button::Listener
{
public:
    //==============================================================================
    GuiEditorComponent ();
    ~GuiEditorComponent() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void setConfigUpdatedCallback(std::function<void(std::string)> callback);
    void setDisplayedConfig(const std::string& value);
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    std::function<void(std::string)> configUpdatedCallback;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::TextEditor> juce_config_textEditor;
    std::unique_ptr<juce::TextButton> juce_applyConfig_textButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiEditorComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

