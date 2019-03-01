#include "pch.h"
#include "MoveEventArgs.h"

using namespace Microsoft::Mixer::MixPlay;

MoveEventArgs::MoveEventArgs(float x, float y, Platform::String^ participantId)
{
	X = x;
	Y = y;
	ParticipantId = participantId;
}
