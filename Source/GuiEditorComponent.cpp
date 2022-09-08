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

#include "GuiEditorComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
GuiEditorComponent::GuiEditorComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    setName ("EditorComponent");
    juce_config_textEditor.reset (new juce::TextEditor ("configEditor"));
    addAndMakeVisible (juce_config_textEditor.get());
    juce_config_textEditor->setMultiLine (true);
    juce_config_textEditor->setReturnKeyStartsNewLine (true);
    juce_config_textEditor->setReadOnly (false);
    juce_config_textEditor->setScrollbarsShown (true);
    juce_config_textEditor->setCaretVisible (true);
    juce_config_textEditor->setPopupMenuEnabled (true);
    juce_config_textEditor->setColour (juce::TextEditor::outlineColourId, juce::Colour (0x00000000));
    juce_config_textEditor->setText (TRANS("Paste your configuration XML here..."));

    juce_applyConfig_textButton.reset (new juce::TextButton ("applyConfigButton"));
    addAndMakeVisible (juce_applyConfig_textButton.get());
    juce_applyConfig_textButton->setButtonText (TRANS("Apply"));
    juce_applyConfig_textButton->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (300, 300);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

GuiEditorComponent::~GuiEditorComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    juce_config_textEditor = nullptr;
    juce_applyConfig_textButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void GuiEditorComponent::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void GuiEditorComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    juce_config_textEditor->setBounds (0, 0, getWidth() - 0, getHeight() - 32);
    juce_applyConfig_textButton->setBounds (0, getHeight() - 28, proportionOfWidth (1.0000f), 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void GuiEditorComponent::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == juce_applyConfig_textButton.get())
    {
        //[UserButtonCode_juce_applyConfig_textButton] -- add your button handler code here..
        if(configUpdatedCallback) configUpdatedCallback(juce_config_textEditor->getText().toStdString());
        //[/UserButtonCode_juce_applyConfig_textButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void GuiEditorComponent::setConfigUpdatedCallback(std::function<void(std::string)> callback)
{
    configUpdatedCallback = callback;
}
void GuiEditorComponent::setDisplayedConfig(const std::string& value)
{
    juce_config_textEditor->setText(value);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="GuiEditorComponent" componentName="EditorComponent"
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="300" initialHeight="300">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TEXTEDITOR name="configEditor" id="39803e87130fd2c4" memberName="juce_config_textEditor"
              virtualName="" explicitFocusOrder="0" pos="0 0 0M 32M" outlinecol="0"
              initialText="Paste your configuration XML here..." multiline="1"
              retKeyStartsLine="1" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
  <TEXTBUTTON name="applyConfigButton" id="f3f44d1781e13928" memberName="juce_applyConfig_textButton"
              virtualName="" explicitFocusOrder="0" pos="0 28R 100% 24" buttonText="Apply"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

