#include "pch.h"
#include "Interactive.h"

#include "Control.h"
#include "ControlProperty.h"
#include "Conversion.h"
#include "CustomEventArgs.h"
#include "ErrorEventArgs.h"
#include "Group.h"
#include "InputEventArgs.h"
#include "LaunchEventArgs.h"
#include "MoveEventArgs.h"
#include "StateChangedEventArgs.h"
#include <..\..\..\interactive-cpp\source\interactivity.h>

using namespace Microsoft::Mixer::MixPlay;
using namespace Platform;
using namespace concurrency;

struct InteractiveImpl
{
	interactive_session m_session;
	std::string m_clientId;
	std::string m_interactiveId;
	std::unique_ptr<std::thread> m_interactiveThread;
	std::map<std::string, std::string> m_controlsByTransaction;
	std::map<std::string, std::string> m_participantByTransaction;
	std::map<std::string, CustomEventArgs^> m_argsByTransaction;
	Collections::Vector<Group^>^ pGroups;
	Collections::Vector<Control^>^ pControls;

	bool m_appIsRunning;
};

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

Interactive::Interactive()
{
	_pImpl = new InteractiveImpl();

	::interactive_config_debug(interactive_debug_trace, [](const interactive_debug_level dbgMsgType, const char *dbgMsg, size_t dbgMsgSize)
	{
		std::string message(dbgMsg);
		message += "\n";
		::OutputDebugStringA(message.c_str());
	});
}

Interactive::~Interactive()
{
	Disconnect();

	delete _pImpl;
}

void Interactive::Disconnect()
{
	if (_pImpl->m_session == nullptr) return;

	::interactive_close_session(_pImpl->m_session);

	_pImpl->m_appIsRunning = false;

	_pImpl->m_interactiveThread->join();

	_pImpl->m_session = nullptr;
}


void Interactive::OnError(void* context, interactive_session session, int errorCode, const char* errorMessage, size_t errorMessageLength)
{
	Interactive^ pThis = reinterpret_cast<Interactive^>(context);

	std::string debugLine = "Mixer error " + std::to_string(errorCode) + ": " + errorMessage + "\r\n";

	auto args = ref new ErrorEventArgs();
	args->Error = errorCode;

	args->Message = ref new Platform::String(converter.from_bytes(errorMessage).c_str());

	pThis->Error(pThis, args);
	OutputDebugStringA(debugLine.c_str());
}

bool Interactive::IsRunning::get()
{
	return _pImpl->m_appIsRunning;
}

int update(interactive_session session)
{
	// This call processes any waiting messages from the interactive service. If there are no messages this returns immediately.
	// All previously registered session callbacks will be called from this thread.
	return interactive_run(session, 1);
}

Platform::String^
Interactive::GetUserName(Platform::String^ participantId)
{
	auto participant = ::ToString(participantId);

	char userName[255];
	size_t userNameLength = 255;

	auto result = ::interactive_participant_get_user_name(
		_pImpl->m_session,
		participant.c_str(), userName, &userNameLength);

	if (result != MIXER_OK) throw ref new COMException(result);

	return ToPlatformString(userName, userNameLength);
}


void Interactive::OnInput(void* context, interactive_session session, const interactive_input* input)
{
	Interactive^ pThis = reinterpret_cast<Interactive^>(context);

	if ((input_type_key == input->type || input_type_click == input->type) && interactive_button_action_down == input->buttonData.action)
	{
		// Capture the transaction on button down to deduct sparks
		if (input->transactionIdLength > 0)
		{
			pThis->_pImpl->m_controlsByTransaction[input->transactionId] = input->control.id;
			pThis->_pImpl->m_participantByTransaction[input->transactionId] = input->participantId;

			interactive_capture_transaction(session, input->transactionId);
		}
	}
	else if (input->type == input_type_move)
	{
		auto participantId = ::ToPlatformString(input->participantId, input->participantIdLength);

		auto args = ref new MoveEventArgs(input->coordinateData.x, input->coordinateData.y, participantId);

		args->ControlId = ::ToPlatformString(input->control.id, input->control.idLength);

		pThis->Moved(pThis, args);
	}
	else if (input->type == input_type_custom)
	{
		if (input->transactionIdLength > 0)
		{
			auto jsonData = ::ToPlatformString(input->jsonData, input->jsonDataLength);

			auto jsonObject = Windows::Data::Json::JsonObject::Parse(jsonData);

			auto inputObject = jsonObject->GetNamedObject("input");

			auto args = ref new CustomEventArgs();

			args->ControlId = ::ToPlatformString(input->control.id, input->control.idLength);
			args->EventName = inputObject->GetNamedString("event");
			args->ParticipantId = ::ToPlatformString(input->participantId, input->participantIdLength);
			args->Value = inputObject->GetNamedString("value");

			pThis->_pImpl->m_argsByTransaction[input->transactionId] = args;

			interactive_capture_transaction(session, input->transactionId);
		}
	}
}

void Interactive::OnTransactionComplete(void* context, interactive_session session, const char* transactionId, size_t transactionIdLength, unsigned int error, const char* errorMessage, size_t errorMessageLength)
{
	Interactive^ pThis = reinterpret_cast<Interactive^>(context);

	if (error)
	{
		std::cerr << errorMessage << " (" << std::to_string(error) << ")";
	}
	else
	{
		auto i = pThis->_pImpl->m_argsByTransaction.find(transactionId);

		if (i == pThis->_pImpl->m_argsByTransaction.end())
		{
			auto particpantId = pThis->_pImpl->m_participantByTransaction[transactionId];

			auto args = ref new InputEventArgs();

			std::string control = pThis->_pImpl->m_controlsByTransaction[transactionId].c_str();

			args->ControlId = ref new Platform::String(std::wstring(control.begin(), control.end()).c_str());

			args->ParticipantId = ::ToPlatformString(particpantId.c_str(), particpantId.length());

			pThis->InputReceived(pThis, args);

			pThis->_pImpl->m_controlsByTransaction.erase(transactionId);
		}
		else
		{
			auto args = i->second;

			pThis->CustomEventTriggered(pThis, args);

			pThis->_pImpl->m_argsByTransaction.erase(i);
		}
	}



}


Windows::Foundation::IAsyncOperation<int>^ Interactive::StartupAsync(Platform::String^ clientId, Platform::String^ interactiveId, Platform::String^ shareCode)
{
	_pImpl->m_appIsRunning = true;

	return create_async([this, clientId, interactiveId, shareCode]
	{
		if (_pImpl->m_session != nullptr) return (int) MIXER_OK;

		std::wstring wsstr(clientId->Data());

		std::wstring winteractiveId(interactiveId->Data());

		_pImpl->m_interactiveId = std::string(winteractiveId.begin(), winteractiveId.end());

		_pImpl->m_clientId = std::string(wsstr.begin(), wsstr.end());

		int err = 0;
		char shortCode[7];
		size_t shortCodeLength = sizeof(shortCode);
		char shortCodeHandle[1024];
		size_t shortCodeHandleLength = sizeof(shortCodeHandle);

		err = interactive_auth_get_short_code(_pImpl->m_clientId.c_str(), nullptr, shortCode, &shortCodeLength, shortCodeHandle, &shortCodeHandleLength);

		if (err) return err;

		std::wstring oauthUrl = converter.from_bytes(std::string("https://www.mixer.com/go?code=") + shortCode);

		Windows::Foundation::Uri^ uri = ref new Windows::Foundation::Uri(Platform::StringReference(oauthUrl.c_str()));

		Launch(this, ref new LaunchEventArgs(uri));

		_pImpl->m_interactiveThread = std::make_unique<std::thread>(std::thread([&, winteractiveId, shortCodeHandle, shareCode]
		{
			try
			{
				std::string shareCode2 = ::ToString(shareCode);

				std::wstring winteractiveId2(winteractiveId);
				int err = 0;

				// Get an authorization token for the user to pass to the connect function.
				std::string authorization;
				err = authorize(authorization, shortCodeHandle);
				if (err)
				{
					throw err;
					return;
				}

				// Connect to the user's interactive channel, using the interactive project specified by the version ID.
				interactive_session session;


				err = interactive_open_session(&session);
				if (err) throw err;

				this->_pImpl->m_session = session;

				err = interactive_set_error_handler(session, OnError);
				if (err) throw err;

				err = interactive_set_session_context(session, (void*)this);// &m_controlsByTransaction);
				if (err) throw err;

				// Register a callback for button presses.
				err = interactive_set_input_handler(session, OnInput);

				if (err) throw err;

				err = interactive_set_transaction_complete_handler(session, OnTransactionComplete);

				if (err) throw err;

				err = interactive_set_state_changed_handler(session, OnStateChanged);

				std::string versionId = std::string(winteractiveId2.begin(), winteractiveId2.end());

				err = interactive_connect(session, authorization.c_str(), versionId.c_str(), shareCode2.c_str(), true);
				if (err) throw err;

				while (_pImpl->m_appIsRunning) /// break out when app terminates
				{
					int err = update(session);
					if (err) break;
					std::this_thread::sleep_for(std::chrono::milliseconds(16));
				}
			}
			catch (int)
			{
			}
		}));

		return 0;
	});
}

std::map<interactive_state, InteractiveState> lookup;

void Interactive::OnStateChanged(void *context, interactive_session session, interactive_state previousState, interactive_state newState)
{
	if (lookup.empty())
	{
		lookup[interactive_disconnected] = InteractiveState::interactive_disconnected;
		lookup[interactive_connecting] = InteractiveState::interactive_connecting;
		lookup[interactive_connected] = InteractiveState::interactive_connected;
		lookup[interactive_ready] = InteractiveState::interactive_ready;
	}

	Interactive^ pThis = reinterpret_cast<Interactive^>(context);

	auto args = ref new StateChangedEventArgs{};

	args->PreviousState = lookup[previousState];
	args->NewState = lookup[newState];

	pThis->StateChanged(pThis, args);
}


int Interactive::authorize(std::string& authorization, const char* shortCodeHandle)
{
	/*int err = 0;
	char shortCode[7];
	size_t shortCodeLength = sizeof(shortCode);
	char shortCodeHandle[1024];
	size_t shortCodeHandleLength = sizeof(shortCodeHandle);
	err = interactive_auth_get_short_code(m_clientId.c_str(), nullptr, shortCode, &shortCodeLength, shortCodeHandle, &shortCodeHandleLength);
	if (err) return err;

	std::wstring oauthUrl = converter.from_bytes(std::string("https://www.mixer.com/go?code=") + shortCode);
	concurrency::task<bool> task(Windows::System::Launcher::LaunchUriAsync(ref new Windows::Foundation::Uri(Platform::StringReference(oauthUrl.c_str()))));
	task.wait();*/

	// Wait for OAuth token response.
	char refreshTokenBuffer[1024];
	size_t refreshTokenLength = sizeof(refreshTokenBuffer);
	int err = interactive_auth_wait_short_code(_pImpl->m_clientId.c_str(), nullptr, shortCodeHandle, refreshTokenBuffer, &refreshTokenLength);
	if (err)
	{
		if (MIXER_ERROR_TIMED_OUT == err)
		{
			std::cout << "Authorization timed out, user did not approve access within the time limit.";
		}
		else if (MIXER_ERROR_AUTH_DENIED == err)
		{
			std::cout << "User denied access.";
		}

		return err;
	}

	/*
	*	TODO:	This is where you would serialize the refresh token for future use in a way that is associated with the current user.
	*			Future calls would then only need to check if the token is stale, refresh it if so, and then parse the new authorization header.
	*/

	// Extract the authorization header from the refresh token.
	char authBuffer[1024];
	size_t authBufferLength = sizeof(authBuffer);
	err = interactive_auth_parse_refresh_token(refreshTokenBuffer, authBuffer, &authBufferLength);
	if (err) return err;

	authorization = std::string(authBuffer, authBufferLength);
	return 0;
}

void Interactive::CreateGroup(Platform::String^ groupId, Platform::String^ sceneId)
{
	auto group = ::ToString(groupId);

	auto scene = ::ToString(sceneId);

	auto result = ::interactive_create_group(_pImpl->m_session, group.c_str(), scene.c_str());

	if (result != MIXER_OK) throw ref new COMException(result);
}

void Interactive::SetGroup(Platform::String^ groupId, Platform::String^ participantId)
{
	auto group = ::ToString(groupId);

	auto participant = ::ToString(participantId);

	auto result = ::interactive_participant_set_group(_pImpl->m_session, participant.c_str(), group.c_str());

	if (result != MIXER_OK) throw ref new COMException(result);
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<Group^>^>^ Interactive::GetGroupsAsync()
{
	return create_async([this]()
	{
		_pImpl->pGroups = ref new Collections::Vector<Group^>();

		auto result = ::interactive_get_groups(_pImpl->m_session, [](void* context, interactive_session session, const interactive_group* group)
		{
			Interactive^ pThis = reinterpret_cast<Interactive^>(context);

			auto newGroup = ref new Group(group);

			pThis->_pImpl->pGroups->Append(newGroup);
		});

		if (result != MIXER_OK) throw ref new COMException(result);

		Windows::Foundation::Collections::IVector<Group^>^ vect = _pImpl->pGroups;

		return vect;
	});
}

std::map<interactive_property_type, Microsoft::Mixer::MixPlay::PropertyType> _typeMap;

Windows::Foundation::Collections::IVector<ControlProperty^>^ ::Interactive::GetProperties(Platform::String^ controlId)
{
	if (_typeMap.empty())
	{
		_typeMap[interactive_unknown_t] = Microsoft::Mixer::MixPlay::PropertyType::interactive_unknown_t;
		_typeMap[interactive_int_t] = Microsoft::Mixer::MixPlay::PropertyType::interactive_int_t;
		_typeMap[interactive_bool_t] = Microsoft::Mixer::MixPlay::PropertyType::interactive_bool_t;
		_typeMap[interactive_float_t] = Microsoft::Mixer::MixPlay::PropertyType::interactive_float_t;
		_typeMap[interactive_string_t] = Microsoft::Mixer::MixPlay::PropertyType::interactive_string_t;
		_typeMap[interactive_array_t] = Microsoft::Mixer::MixPlay::PropertyType::interactive_array_t;
		_typeMap[interactive_object_t] = Microsoft::Mixer::MixPlay::PropertyType::interactive_object_t;
	}
	auto control = ::ToString(controlId);

	size_t length = 0;

	auto result = ::interactive_control_get_property_count(_pImpl->m_session, control.c_str(), &length);

	if (result != MIXER_OK) throw ref new COMException(result);

	auto properties = ref new Collections::Vector<ControlProperty^>();

	for (size_t index = 0; index < length; index++)
	{
		char propName[255];
		size_t propNameLength = 255;
		interactive_property_type propertyType;

		result = ::interactive_control_get_property_data(_pImpl->m_session, control.c_str(), index, propName, &propNameLength, &propertyType);

		if (result != MIXER_OK) throw ref new COMException(result);

		auto property = ref new ControlProperty(_pImpl->m_session, control.c_str());

		property->Name = ::ToPlatformString(propName, propNameLength);
		property->PropertyType = _typeMap[propertyType];

		properties->Append(property);
	}

	return properties;
}

Windows::Foundation::Collections::IVector<Control^>^ Interactive::GetControls(Platform::String^ sceneId) 
{
	_pImpl->pControls = ref new Collections::Vector<Control^>();

	auto scene = ::ToString(sceneId);

	auto result = ::interactive_scene_get_controls(_pImpl->m_session, scene.c_str(), [](void *context, interactive_session session, const interactive_control *control)
	{
		Interactive^ pThis = reinterpret_cast<Interactive^>(context);

		auto newControl = ref new Control();

		newControl->Id = ::ToPlatformString(control->id, control->idLength);
		newControl->Kind = ::ToPlatformString(control->kind, control->kindLength);

		pThis->_pImpl->pControls->Append(newControl);
	});

	if (result != MIXER_OK) throw ref new COMException(result);

	return _pImpl->pControls;
}

ControlProperty^ Interactive::GetControlProperty(Platform::String^ controlId, Platform::String^ propertyName)
{
	auto control = ::ToString(controlId);

	auto property = ref new ControlProperty(_pImpl->m_session, control.c_str());

	property->Name = propertyName;

	return property;
}

