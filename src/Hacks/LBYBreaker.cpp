#include "lby.h"
#include "exploits.h"
#include "antiaim.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IClientEntity.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/entity.h"
#include "../interfaces.h"

//this is here for now...for reasons

float LBY::GetMaxDelta(CCSGOAnimState *animState) {

    float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

    float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

    float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
    float unk2 = unk1 + 1.0f;
    float delta;

    if (animState->duckProgress > 0)
    {
        unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2));// - 1.f
    }

    delta = *(float*)((uintptr_t)animState + 0x3A4) * unk2;

    return delta - 0.5f;
}


void LBY::CreateMove(CUserCmd* cmd)
{

  C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
  QAngle angle = cmd->viewangles;
  static bool directionSwitch = false;

  if (inputSystem->IsButtonDown(Settings::AntiAim::left))
		directionSwitch = true;
	else if (inputSystem->IsButtonDown(Settings::AntiAim::right))
		directionSwitch = false;

  CCSGOAnimState* animState = localplayer->GetAnimState();

  float maxDelta = LBY::GetMaxDelta(animState);
  float lbyOffset = Settings::AntiAim::LBYBreaker::enabled ? Settings::AntiAim::LBYBreaker::offset : maxDelta;

  bool needToFlick = false;

  static float lastCheck;
  static bool lbyBreak = false;

  if(Settings::AntiAim::LBYBreaker::enabled){
    if (localplayer->GetVelocity().Length2D() >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN)
    {
        lbyBreak = false;
        lastCheck = globalVars->curtime;
    }
    else
    {
        if (!lbyBreak && (globalVars->curtime - lastCheck) > 0.22)
        {
            angle.y += directionSwitch ? -122 : 122;
            lbyBreak = true;
            lastCheck = globalVars->curtime;
            needToFlick = true;
        }
        else if (lbyBreak && (globalVars->curtime - lastCheck) > 1.1)
        {
            angle.y += directionSwitch ? -122 : 122;
            lbyBreak = true;
            lastCheck = globalVars->curtime;
            needToFlick = true;
        }
    }
  }/*
    if (needToFlick)
        CreateMove::sendPacket = false;
    else
    {
      static AntiAimType type;
      CreateMove::sendPacket = true;

      AntiAim::CreateMove(cmd);
   }*/
}

