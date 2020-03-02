#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../settings.h"
#include "../interfaces.h"
#include "animFix.h"
#include "resolver.h"

template< class T, class Y >
T Clamp( T const &val, Y const &minVal, Y const &maxVal )
{
	if( val < minVal )
		return minVal;
	else if( val > maxVal )
		return maxVal;
	else
		return val;
}

float LagFix()
{
  //tbh screw svcheats
  static ConVar* svcheats = cvar->FindVar("sv_cheats");
  svcheats->SetValue(1);

	float updaterate = cvar->FindVar("cl_updaterate")->GetFloat();
	static ConVar* minupdate = cvar->FindVar("sv_minupdaterate");
	static ConVar* maxupdate = cvar->FindVar("sv_maxupdaterate");

	if (minupdate && maxupdate)
		updaterate = maxupdate->GetFloat();

	float ratio = cvar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = cvar->FindVar("cl_interp")->GetFloat();
	static ConVar* interp = cvar->FindVar("cl_interp");
	static ConVar* interpolate = cvar->FindVar("cl_interpolate");
	static ConVar* cmin = cvar->FindVar("sv_client_min_interp_ratio");
	static ConVar* cmax = cvar->FindVar("sv_client_max_interp_ratio");

	if (cmin && cmax && cmin->GetFloat() != 1)
		ratio = Clamp(ratio, cmin->GetFloat(), cmax->GetFloat());

	interp->SetValue(0);
	interpolate->SetValue(0);
	return std::max(lerp, ratio / updaterate);


}

//credits to Luna for the pAnimFix!!111 b1g f1x! time :clap: to :clap: play :clap: with :clap: code :clap:
void animFix::FrameStageNotify(ClientFrameStage_t stage)
{
  //adding LagFix here too.
  LagFix();

	  C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    for (int i = 1; i <= engine->GetMaxClients(); i++) {
		C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

			if (!player
				|| player == localplayer
				|| player->GetDormant()
				|| !player->GetAlive()
				|| player->GetImmune()
				|| Entity::IsTeamMate(player, localplayer))
				continue;

        // Correct team animations
  		  if (!Entity::IsTeamMate && player != localplayer)
            player->GetAnimState();
	  		  //player->clientSideAnimation() = true;

        const auto state = player->GetAnimState();
        if (!state)
            continue;

        // Make sure this player has not been fixed already
        if (player->GetSimulationTime() == (player->GetSimulationTime() + 0x04))
            continue;

        const Vector newVelocity = player->GetVelocity();
        //const bool isLocalPlayer = player->index() == interfaces->engine->getLocalPlayer();
        const bool isLocalPlayer = localplayer;

        // The amount of ticks choked
        int lag = player->GetSimulationTime() - (player->GetSimulationTime() + 0x04);

        // Fix animations for every tick in between the oldest and newest
        for (int i = 0; i < lag; i++) {
            // Fix feet spinning
            player->GetAnimState()->goalFeetYaw  = 0.f;

            if (lag - 1 == i) {
                player->GetVelocity() = newVelocity;

                if (lag > 1)
                    Resolver::FrameStageNotify(stage);
		    }

            const float backupFrametime = globalVars->frametime;
            const float backupCurtime = globalVars->curtime;

            if (!isLocalPlayer) {
                globalVars->frametime = globalVars->interval_per_tick;
                globalVars->curtime = player->GetSimulationTime();

                // Disable the enemy's dirty velocity flag, for accurate velocity prediction
                !(player->GetFlags() & FL_ONGROUND);
            }

            // Allow re-animating within the same frame
    //        if (player->last_client_side_animation_update_framecount() == globalVars->framecount)
     //           player->last_client_side_animation_update_framecount() - 1;

            // forceUpdateAnimations = player->clientSideAnimation() = true;
           // player->clientSideAnimation() = true;
            player->GetAnimState();
            //player->updateClientSideAnimation();
            // forceUpdateAnimations = false;

            if (!isLocalPlayer)
                !player->GetAnimState();
                //player->clientSideAnimation() = false;


            globalVars->frametime = backupFrametime;
            globalVars->curtime = backupCurtime;
        }
	}
}

void animFix::PostFrameStageNotify(ClientFrameStage_t stage)
{
}

void animFix::FireGameEvent(IGameEvent* event)
{
	if (!event)
		return;

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) != 0 && strcmp(event->GetName(), XORSTR("cs_game_disconnected")) != 0)
		return;

	if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
		return;

}

