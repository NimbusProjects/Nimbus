#include "resolver.h"

#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../settings.h"
#include "../interfaces.h"
#include "antiaim.h"

bool Settings::Resolver::resolveAll = false;
std::vector<int64_t> Resolver::Players = { };

std::vector<std::pair<C_BasePlayer*, QAngle>> player_data;

// New resolver (by Skerei, updated by Zede)
static void Resolve(C_BasePlayer* player, float feetYaw, float angleYaw, float maxDelta)
{
	if (player->GetVelocity().Length() > 75.77f)
    {
        player->GetEyeAngles()->y = *player->GetLowerBodyYawTarget();
        player->GetAnimState()->goalFeetYaw = (180.f + (angleYaw - feetYaw)) / 360.f;
        Math::NormalizeYaw(player->GetEyeAngles()->y);

        angleYaw = (rand() % 2) ? angleYaw + (maxDelta / 2.2f) : angleYaw - (maxDelta / 2.2f);
    }
    else if (player->GetVelocity().Length() < 75.77f)
    {
        player->GetEyeAngles()->y = *player->GetLowerBodyYawTarget();
	    //this is an issue imo, players can break lby still -echo
	    //is there a particular reason for doing this? desync is totally reliant on body poses, rather than old ticks being used to calculate hitboxes
	    //the players eyeangles are fine, we should be fixing body poses for proper resolver.
        player->GetAnimState()->goalFeetYaw = (180.f + (angleYaw - feetYaw)) / 360.f;
	    //issue with this is that it's not a proper goal feet calculation, shouldnt we be using the server side calculations with slight adjustment to it so to account for
	    //those choking ticks?
	    //for example, we can break this simply by micro moving while randomizing when we choke packets to force goal feet yaw to our choked eye angles
	    // -echo pls dont mind the fucked up lines i did this on mobile <3

	
        Math::NormalizeYaw(player->GetEyeAngles()->y);

        if (feetYaw >= -maxDelta & feetYaw < 0)
            player->GetAnimState()->goalFeetYaw -= maxDelta * 0.66f;
	    //Why are we doing this? Players can desync less than maxdelta, and why are we using magic values? -echo
        else
            player->GetAnimState()->goalFeetYaw += maxDelta * 0.66f;

		CUtlVector<AnimationLayer>* layers = player->GetAnimOverlay();

		for (int i = 0; i <= layers->Count(); i++)
		{
			float m_flPlaybackRate = layers->operator[](i).m_flPlaybackRate; 
			//this is still using animation layers to resolve owo -echo

			if (m_flPlaybackRate > 0.1f)
			{
				for (float resolveDelta = 0.0f; resolveDelta < -maxDelta || resolveDelta > maxDelta; resolveDelta = resolveDelta / 2.2f)
					player->GetEyeAngles()->y = resolveDelta;
			}
		}
    }
}

void Resolver::FrameStageNotify(ClientFrameStage_t stage)
{
	if (!engine->IsInGame())
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		for (int i = 1; i < engine->GetMaxClients(); ++i)
		{
			C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

			if (!player
				|| player == localplayer
				|| player->GetDormant()
				|| !player->GetAlive()
				|| player->GetImmune()
				|| Entity::IsTeamMate(player, localplayer))
				continue;

			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			if (!Settings::Resolver::resolveAll && std::find(Resolver::Players.begin(), Resolver::Players.end(), entityInformation.xuid) == Resolver::Players.end())
				continue;

			player_data.push_back(std::pair<C_BasePlayer*, QAngle>(player, *player->GetEyeAngles()));

			//player->GetEyeAngles()->y = *player->GetLowerBodyYawTarget();

			/* Old Fuzion resolver
			player->GetEyeAngles()->y = (rand() % 2) ?
                                        player->GetEyeAngles()->y + (AntiAim::GetMaxDelta(player->GetAnimState()) * 0.66f) :
                                        player->GetEyeAngles()->y - (AntiAim::GetMaxDelta(player->GetAnimState()) * 0.66f);
			*/

			Resolve(player, player->GetAnimState()->currentFeetYaw, player->GetEyeAngles()->y, AntiAim::GetMaxDelta(player->GetAnimState()));
		}
	}
	else if (stage == ClientFrameStage_t::FRAME_RENDER_END)
	{
		for (unsigned long i = 0; i < player_data.size(); i++)
		{
			std::pair<C_BasePlayer*, QAngle> player_aa_data = player_data[i];
			*player_aa_data.first->GetEyeAngles() = player_aa_data.second;
		}

		player_data.clear();
	}
}

void Resolver::PostFrameStageNotify(ClientFrameStage_t stage)
{
}

void Resolver::FireGameEvent(IGameEvent* event)
{
	if (!event)
		return;

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) != 0 && strcmp(event->GetName(), XORSTR("cs_game_disconnected")) != 0)
		return;

	if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
		return;

	Resolver::Players.clear();
}
