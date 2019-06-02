#pragma once
using namespace Platform;


Platform::String^ ToPlatformString(const char* pString, size_t length);

const std::string ToString(Platform::String^ pString);

const std::string ToString(const std::wstring& string_to_convert);

const std::wstring ToWString(const std::string& str);
