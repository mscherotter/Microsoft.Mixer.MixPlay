#pragma once
namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			public ref class ErrorEventArgs sealed
			{
			public:
				ErrorEventArgs();

				property int Error;
				property Platform::String^ Message;
			};
		}
	}
}