#include "pch.h"
#include "Group.h"
#include "Conversion.h"
#include "..\..\..\interactive-cpp\source\interactivity.h"

using namespace Microsoft::Mixer::MixPlay;

Group::Group(const interactive_group* pGroup)
{
	Id = ::ToPlatformString(pGroup->id, pGroup->idLength);

	SceneId = ::ToPlatformString(pGroup->sceneId, pGroup->sceneIdLength);
}
