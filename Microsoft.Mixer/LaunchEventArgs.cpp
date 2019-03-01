#include "pch.h"
#include "LaunchEventArgs.h"

using namespace Microsoft::Mixer::MixPlay;

LaunchEventArgs::LaunchEventArgs(Windows::Foundation::Uri^ uri)
{
	Uri = uri;
}
