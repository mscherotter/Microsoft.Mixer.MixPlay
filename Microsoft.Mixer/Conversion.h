#pragma once
using namespace Platform;

std::string ToString(Platform::String^ pString);

Platform::String^ ToPlatformString(const char* pString, size_t length);

