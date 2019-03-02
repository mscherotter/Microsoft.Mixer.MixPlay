#pragma once

struct InteractiveImpl;
struct interactive_input;
enum interactive_state;
typedef void* interactive_session;

namespace Microsoft
{
	namespace Mixer
	{
		namespace MixPlay
		{
			ref class InputEventArgs;
			ref class LaunchEventArgs;
			ref class MoveEventArgs;
			ref class ErrorEventArgs;
			ref class StateChangedEventArgs;
			ref class CustomEventArgs;
			ref class Group;
			ref class Control;
			ref class ControlProperty;

			public delegate void InputHandler(Platform::Object^ sender, InputEventArgs^ args);
			public delegate void LaunchHandler(Platform::Object^ sender, LaunchEventArgs^ args);
			public delegate void MoveHandler(Platform::Object^ sender, MoveEventArgs^ args);
			public delegate void ErrorHandler(Platform::Object^ sender, ErrorEventArgs^ args);
			public delegate void StateChangedHandler(Platform::Object^ sender, StateChangedEventArgs^ args);
			public delegate void CustomEventHandler(Platform::Object^ sender, CustomEventArgs^ args);

			/// <summary>Mixer Interactive Component</summary>
			/// <remarks>This uses the Mixer C++ interactive SDK https://github.com/mixer/interactive-cpp </remarks>
			public ref class Interactive sealed
			{
			public:
				///<summary>Initializes a new instance of the Interactive class</summary>
				Interactive();

				///<summary>Close the connection to Mixer</summary>
				virtual ~Interactive();

				/// <summary>Gets a value indicating whether the connection to Mixer is open</summary>
				property bool IsRunning
				{
					bool get();
				}

				/// <summary>Disconnects from Mixer.com</summary>
				void Disconnect();

				/// <summary>Startup the interactive SDK and Connect to the Mixer platform</summary>
				/// <param name="clientId">the Mixer OAuth Client ID from https://mixer.com/lab/oauth</param>
				/// <param name="interactiveId">the Mixer MixPlay Version ID (found in the Code tab of the MixPlay editor)</param>
				/// <param name="shareCode">the Mixer MixPlay Share Code (found in the Publish tab of the MixPlay editor)</param>
				Windows::Foundation::IAsyncOperation<int>^ StartupAsync(Platform::String^ clientId, Platform::String^ interactiveId, Platform::String^ shareCode);

				/// <summary>Gets the user name for a participant Id</summary>
				/// <param name="participantId">the participant id</param>
				Platform::String^ GetUserName(Platform::String^ participantId);

				/// <summary>Create a participant group</summary>
				/// <param name="groupId">the group id</param>
				/// <param name="sceneId">the MixPlay scene id</param>
				void CreateGroup(Platform::String^ groupId, Platform::String^ sceneId);

				/// <summary>Sets the group Id for a participant </summary>
				/// <param name="groupId">the group id</param>
				/// <param name="sceneId">the participant id</param>
				void SetGroup(Platform::String^ groupId, Platform::String^ participantId);

				/// <summary>Gets the groups in the MixPlay</summary>
				Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<Group^>^>^ GetGroupsAsync();

				/// <summary>Gets a control property</summary>
				/// <param name="controlId">the control id</param>
				/// <param name="propertyName">the property name</param>
				ControlProperty^ GetControlProperty(Platform::String^ controlId, Platform::String^ propertyName);

				/// <summary>Gets the properties for a specific control</summary>
				/// <param name="controlId">the control Id</param>
				Windows::Foundation::Collections::IVector<ControlProperty^>^ GetProperties(Platform::String^ controlId);

				/// <summary>Gets the control for a specific scene</summary>
				/// <param name="sceneId">the scene id</param>
				Windows::Foundation::Collections::IVector<Control^>^ GetControls(Platform::String^ sceneId);
				
				/// <summary>Launch handler for the app to launch an URI in the UI thread</summary>
				/// <example>
				/// <code>
				/// Interactive.Launch += Interactive_Launch;
				/// ...
				/// private async void Interactive_Launch(object sender, LaunchEventArgs args)
				/// {
				/// 	await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async delegate
				/// 		{
				/// 			await Launcher.LaunchUriAsync(args.Uri);
				/// 		});
				/// }
				/// </code>
				/// </example>
				event LaunchHandler^ Launch;

				/// <summary>Button input handler</summary>
				event InputHandler^ InputReceived;
				/// <summary>Joystick or mouse moved handler</summary>
				event MoveHandler^ Moved;

				/// <summary>Error handler</summary>
				event ErrorHandler^ Error;

				/// <summary>State changed handler</summary>
				event StateChangedHandler^ StateChanged;

				/// <summary>Custom input handler</summary>
				event CustomEventHandler^ CustomEventTriggered;

			private:
				static void OnInput(void* context, interactive_session session, const interactive_input* input);
				
				static void OnError(void* context, interactive_session session, int errorCode, const char* errorMessage, size_t errorMessageLength);
				
				static void OnTransactionComplete(void* context, interactive_session session, const char* transactionId, size_t transactionIdLength, unsigned int error, const char* errorMessage, size_t errorMessageLength);
				
				static void OnStateChanged(void *context, interactive_session session, interactive_state previousState, interactive_state newState);
				
				int Interactive::authorize(std::string& authorization, const char* shortCodeHandle);

				InteractiveImpl* _pImpl;
			};
		}
	}
}
