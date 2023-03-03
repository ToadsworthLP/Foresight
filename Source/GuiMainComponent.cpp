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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "GuiMainComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
GuiMainComponent::GuiMainComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    setName ("MainComponent");

    juce_pluginVersion_label.reset (new juce::Label ("pluginVersion",
                                                          TRANS("Plugin Version")));
    addAndMakeVisible (juce_pluginVersion_label.get());
    juce_pluginVersion_label->setFont (juce::Font (14.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce_pluginVersion_label->setJustificationType (juce::Justification::centred);
    juce_pluginVersion_label->setEditable (false, false, false);
    juce_pluginVersion_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce_pluginVersion_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    juce_pluginVersion_label->setText (CURRENT_VERSION, juce::NotificationType::dontSendNotification);

    juce_currentConfigHeading_label.reset (new juce::Label ("currentConfigHeading",
                                                            TRANS("Loaded Configuration")));
    addAndMakeVisible (juce_currentConfigHeading_label.get());
    juce_currentConfigHeading_label->setFont (juce::Font (14.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce_currentConfigHeading_label->setJustificationType (juce::Justification::centred);
    juce_currentConfigHeading_label->setEditable (false, false, false);
    juce_currentConfigHeading_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce_currentConfigHeading_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce_currentConfigValue_label.reset (new juce::Label ("currentConfigValue",
                                                          TRANS("Config Title")));
    addAndMakeVisible (juce_currentConfigValue_label.get());
    juce_currentConfigValue_label->setFont (juce::Font (24.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce_currentConfigValue_label->setJustificationType (juce::Justification::centred);
    juce_currentConfigValue_label->setEditable (false, false, false);
    juce_currentConfigValue_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce_currentConfigValue_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce_currentLatency_label.reset (new juce::Label ("currentLatency",
                                                      TRANS("Total latency: xxx ms")));
    addAndMakeVisible (juce_currentLatency_label.get());
    juce_currentLatency_label->setFont (juce::Font (14.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce_currentLatency_label->setJustificationType (juce::Justification::centred);
    juce_currentLatency_label->setEditable (false, false, false);
    juce_currentLatency_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce_currentLatency_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (300, 300);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

GuiMainComponent::~GuiMainComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    juce_pluginVersion_label = nullptr;
    juce_currentConfigHeading_label = nullptr;
    juce_currentConfigValue_label = nullptr;
    juce_currentLatency_label = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void GuiMainComponent::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void GuiMainComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    juce_pluginVersion_label->setBounds (proportionOfWidth (0.0000f), (getHeight() / 2) + -88, proportionOfWidth (1.0000f), 24);
    juce_currentConfigHeading_label->setBounds (proportionOfWidth (0.0000f), (getHeight() / 2) + -54, proportionOfWidth (1.0000f), 24);
    juce_currentConfigValue_label->setBounds (proportionOfWidth (0.0000f), (getHeight() / 2) + -30, proportionOfWidth (1.0000f), 24);
    juce_currentLatency_label->setBounds (proportionOfWidth (0.0000f), (getHeight() / 2) + 34, proportionOfWidth (1.0000f), 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void GuiMainComponent::setDisplayedConfigName(const std::string& value)
{
    juce_currentConfigValue_label->setText(value, juce::NotificationType::sendNotification);
}

void GuiMainComponent::setDisplayedLatency(const std::string& value)
{
    juce_currentLatency_label->setText(value, juce::NotificationType::sendNotification);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="GuiMainComponent" componentName="MainComponent"
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="300" initialHeight="300">
  <BACKGROUND backgroundColour="ff323e44"/>
  <LABEL name="currentConfigValue" id="8c0a70b115414666" memberName="juce_currentConfigValue_label"
         virtualName="" explicitFocusOrder="0" pos="0% -30C 100% 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Config Title" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="24.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="currentConfigHeading" id="6ceebb525fcda17b" memberName="juce_currentConfigHeading_label"
         virtualName="" explicitFocusOrder="0" pos="0% -54C 100% 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Loaded Configuration" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="14.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="currentLatency" id="25fccc4eac7ef8da" memberName="juce_currentLatency_label"
         virtualName="" explicitFocusOrder="0" pos="0% 34C 100% 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Total latency: xxx ms" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="14.0" kerning="0.0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

