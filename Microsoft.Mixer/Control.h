#pragma once

namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			public ref class Control sealed
			{
			public:
				Control();
				property Platform::String^ Kind;

				property Platform::String^ Id;
			};
		}
	}
}

