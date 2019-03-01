#pragma once

struct interactive_group;

namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			public ref class Group sealed
			{
			public:
				property Platform::String^ Id;
				property Platform::String^ SceneId;
			internal :
				Group(const interactive_group* pGroup);
			};

		}
	}
}

