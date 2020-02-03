#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/CBaseClientState.h"

namespace FakeCrouch
{
	//Hooks
	void CreateMove(CUserCmd* cmd);
	void OverrideView(CViewSetup *pSetup);
};
