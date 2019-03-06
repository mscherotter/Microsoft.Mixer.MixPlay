#pragma once

namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			public ref class InputEventArgs sealed
			{
			public:
				InputEventArgs();

				property Platform::String^ ControlId;
				property Platform::String^ ParticipantId;
				property bool	IsDown;
			};
		}
	}
}
