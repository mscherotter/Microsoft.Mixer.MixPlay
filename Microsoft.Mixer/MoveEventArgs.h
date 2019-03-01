#pragma once
namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			public ref class MoveEventArgs sealed
			{
			public:
				MoveEventArgs(float x, float y, Platform::String^ participantId);

				property float X;
				property float Y;
				property Platform::String^ ParticipantId;
				property Platform::String^ ControlId;
			};
		}
	}
}