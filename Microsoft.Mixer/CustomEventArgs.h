#pragma once

namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			public ref class CustomEventArgs sealed
			{
			public:
				CustomEventArgs();

				property Platform::String^ EventName;
				property Platform::String^ Value;
				property Platform::String^ ControlId;
				property Platform::String^ ParticipantId;
			};
		}
	}
}