#include "InputTreeCase.h"

InputTreeCase::InputTreeCase(const juce::XmlElement& source)
{
	equals = source.getIntAttribute("equals", -1);
	greater = source.getIntAttribute("greater", INT_MIN);
	less = source.getIntAttribute("less", INT_MAX);

	alwaysTrue = source.isTextElement();
}

bool InputTreeCase::check(int value)
{
	return alwaysTrue || (equals != -1 ? (value == equals) : (value > greater && value < less));
}
