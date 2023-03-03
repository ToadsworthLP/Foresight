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
class GuiMainComponent  : public juce::Component
{
public:
    //==============================================================================
    GuiMainComponent ();
    ~GuiMainComponent() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void setDisplayedConfigName(const std::string& value);
    void setDisplayedLatency(const std::string& value);
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> juce_pluginVersion_label;
    std::unique_ptr<juce::Label> juce_currentConfigValue_label;
    std::unique_ptr<juce::Label> juce_currentConfigHeading_label;
    std::unique_ptr<juce::Label> juce_currentLatency_label;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiMainComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

