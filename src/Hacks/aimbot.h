#pragma once

#include <vector>
#include <cstdint>
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include "../SDK/IClientEntity.h"

class INetChannel;

namespace Aimbot
{
	extern bool aimStepInProgress;
	extern std::vector<int64_t> friends;
  extern int targetAimbot;
  //extern C_BasePlayer* curtarget;

	//Hooks

	void PrePredictionCreateMove(CUserCmd* cmd);
	void CreateMove(CUserCmd* cmd);
	void FireGameEvent(IGameEvent* event);
	void UpdateValues();
}
