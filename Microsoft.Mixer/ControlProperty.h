#pragma once

typedef void* interactive_session;	

namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			public enum class PropertyType
			{
				interactive_unknown_t, 
				interactive_int_t, 
				interactive_bool_t, 
				interactive_float_t,
				interactive_string_t, 
				interactive_array_t, 
				interactive_object_t
			};

			public ref class ControlProperty sealed
			{
			public:
				property Platform::String^ Name;
				property PropertyType PropertyType;
				
				property int Integer
				{
					int get();
				}

				property Platform::String^ String
				{
					Platform::String^ get();

					void set(Platform::String^ value);
				}

			internal:
				ControlProperty(interactive_session session, const char* controlId);

			private:
				interactive_session _session;
				std::string _controlId;
			};
		}
	}
}