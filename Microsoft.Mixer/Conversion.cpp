#include "pch.h"
#include "Conversion.h"

const std::string ToString(Platform::String^ pString)
{
	std::wstring wstring1(pString->Data());

	return ToString(wstring1);
}

Platform::String^ ToPlatformString(const char* pString, size_t length)
{
	auto wString = std::wstring(pString, pString + length);

	return ref new Platform::String(wString.c_str());
}

const std::string ToString(const std::wstring& string_to_convert)
{
	using convert_type = std::codecvt_utf8<wchar_t>;

	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.to_bytes(string_to_convert);
}

const std::wstring ToWString(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}
