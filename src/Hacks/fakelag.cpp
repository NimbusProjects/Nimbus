#include "fakelag.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"

bool Settings::FakeLag::enabled = false;
int Settings::FakeLag::value = 9;
bool Settings::FakeLag::adaptive = false;

int Settings::FakeLag::minTicks = 0; //dw bout this for now
int Settings::FakeLag::maxTicks = 9;

bool Settings::FakeLag::lagSpike = false;
bool FakeLag::lagSpike = false;

bool Settings::FakeLag::States::enabled = false;
bool Settings::FakeLag::States::Standing::enabled = false;
int Settings::FakeLag::States::Standing::value = 0;
bool Settings::FakeLag::States::Moving::enabled = false;
int Settings::FakeLag::States::Moving::value = 0;
bool Settings::FakeLag::States::Air::enabled = false;
int Settings::FakeLag::States::Air::value = 0;

//this file is gonna get cleaned up s00n
static int ticks = 0;
int ticksMax = 16;

void FakeLag::CreateMove(CUserCmd* cmd, bool bSend)
{


  if (!Settings::FakeLag::enabled)
    return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->GetAlive())
		return;


   if(Settings::FakeLag::lagSpike && localplayer->GetVelocity().Length() >= 1.0f &&  localplayer->GetVelocity().Length() <= 35.0f && (localplayer->GetFlags() & FL_ONGROUND))
   {
	  CreateMove::sendPacket2 = ticks < 16;
    return;
  }


    if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK)
      CreateMove::sendPacket2 = true;


	if (ticks >= ticksMax)
	{
		CreateMove::sendPacket2 = true;
		ticks = 0;
	}
	else if (FakeLag::lagSpike)
		CreateMove::sendPacket2 = false;
	else if (Settings::FakeLag::States::enabled)
	{
		if (Settings::FakeLag::States::Air::enabled && !(localplayer->GetFlags() & FL_ONGROUND))
			CreateMove::sendPacket2 = ticks < 16 - Settings::FakeLag::States::Air::value;
		else if (Settings::FakeLag::States::Moving::enabled && localplayer->GetVelocity().Length() > 150.f)
			CreateMove::sendPacket2 = ticks < 16 - Settings::FakeLag::States::Moving::value;
		else if(Settings::FakeLag::States::Standing::enabled)
			CreateMove::sendPacket2 = ticks < 16 - Settings::FakeLag::States::Standing::value;
	}
	else
		CreateMove::sendPacket2 = ticks < 16 - Settings::FakeLag::value;

	ticks++;
  }

