#include "pch.h"
#include "Conversion.h"

std::string ToString(Platform::String^ pString)
{
	std::wstring wstring1(pString->Data());

	std::string string1(wstring1.begin(), wstring1.end());

	return string1;
}

Platform::String^ ToPlatformString(const char* pString, size_t length)
{
	auto wString = std::wstring(pString, pString + length);

	return ref new Platform::String(wString.c_str());
}
