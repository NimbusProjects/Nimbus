#pragma once

#include <vector>
#include <cstdint>
#include "../SDK/definitions.h"
#include "../SDK/IGameEvent.h"

namespace animFix
{
	//Hooks
  void FrameStageNotify(ClientFrameStage_t stage);
	void PostFrameStageNotify(ClientFrameStage_t stage);
	void FireGameEvent(IGameEvent* event);
}
