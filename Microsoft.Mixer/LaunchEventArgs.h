#pragma once


namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			public ref class LaunchEventArgs sealed
			{
			public:
				LaunchEventArgs(Windows::Foundation::Uri^ uri);

				property Windows::Foundation::Uri^ Uri;
			};
		}
	}
}

