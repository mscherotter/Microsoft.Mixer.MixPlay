#pragma once

namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			public enum class InteractiveState
			{
				interactive_disconnected,
				interactive_connecting,
				interactive_connected,
				interactive_ready
			};

			public ref class StateChangedEventArgs sealed
			{
			public:
				StateChangedEventArgs();

				property InteractiveState PreviousState;
				property InteractiveState NewState;

			};
		}
	}
}
