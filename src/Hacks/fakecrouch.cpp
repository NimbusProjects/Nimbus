#include "fakecrouch.h"
#include "thirdperson.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
//#include "../SDK/IClientEntity.h"

bool Settings::FakeCrouch::enabled = false;
ButtonCode_t Settings::FakeCrouch::key = ButtonCode_t::KEY_NONE;

static bool counter = false;
static int counters = 0;

void FakeCrouch::CreateMove(CUserCmd* cmd)
{
    if (!Settings::FakeCrouch::enabled)
        return;
    else
        cmd->buttons |= IN_BULLRUSH;

    if(!engine->IsInGame())
        return;

    if (!inputSystem->IsButtonDown(Settings::FakeCrouch::key))
        return;

    if (counters == 14)
    {
        counters = 0;
        counter = !counter;
    }
    counters++;

    CreateMove::sendPacket = false;

    if (counter)
    {
        cmd->buttons |= IN_DUCK;
        CreateMove::sendPacket = true;
    }
    else
        cmd->buttons &= ~IN_DUCK;
}


void FakeCrouch::OverrideView(CViewSetup *pSetup)
{

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    if(!engine->IsInGame())
        return;

    if(!Settings::FakeCrouch::enabled)
      return;

    if (!inputSystem->IsButtonDown(Settings::FakeCrouch::key))
        return;

    if(localplayer || localplayer->GetAlive())
      pSetup->origin.z = localplayer->GetAbsOrigin().z + 64.f;
}

