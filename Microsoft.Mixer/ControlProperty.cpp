#include "pch.h"
#include "ControlProperty.h"
#include "Conversion.h"
#include "..\..\..\interactive-cpp\source\interactivity.h"

using namespace Microsoft::Mixer::MixPlay;

ControlProperty::ControlProperty(interactive_session session, const char* controlId)
{
	_session = session;
	_controlId = controlId;
}

int ControlProperty::Integer::get()
{
	auto key = ::ToString(Name);

	int value;

	auto result = ::interactive_control_get_property_int(_session, _controlId.c_str(), key.c_str(), &value);

	if (result != MIXER_OK) throw ref new COMException(result);

	return value;
}

Platform::String^ ControlProperty::String::get()
{
	auto key = ::ToString(Name);

	char value[255];

	size_t valueLength = 255;

	auto result = ::interactive_control_get_property_string(_session, _controlId.c_str(), key.c_str(), value, &valueLength);

	if (result != MIXER_OK) throw ref new COMException(result);

	return ::ToPlatformString(value, valueLength);
}

void ControlProperty::String::set(Platform::String^ value)
{
	auto key = ::ToString(Name);
	auto property = ::ToString(value);
	auto result = ::interactive_control_set_property_string(_session, _controlId.c_str(), key.c_str(), property.c_str());

	if (result != MIXER_OK) throw ref new COMException(result);
}

