#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"

namespace FakeVote
{
	//Hooks
	void CreateMove(CUserCmd* cmd);
	//void FireGameEvent(IGameEvent* event);
	void CallVote(int type, int userid);
}
