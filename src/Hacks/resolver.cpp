#include "resolver.h"

#include <math.h>
#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../settings.h"
#include "../interfaces.h"
#include "antiaim.h"

bool Settings::Resolver::resolveAll = false;
float Settings::Resolver::resolveDelt = 0.5f;
std::vector<int64_t> Resolver::Players = { };

std::vector<std::pair<C_BasePlayer*, QAngle>> player_data;

// New resolver (by Skerei, updated by Zede)
/*static void Resolve(C_BasePlayer* player, float feetYaw, float angleYaw, float maxDelta)
{


    if (player->GetVelocity().Length() > 75.77f)
    {
        float LBY = *player->GetLowerBodyYawTarget();
  			player->GetEyeAngles()->y = (LBY + (angleYaw - feetYaw)) / 360.0f;
        player->GetAnimState()->goalFeetYaw = (180.f + (angleYaw - feetYaw)) / 360.f;
        Math::NormalizeYaw(player->GetEyeAngles()->y);

        angleYaw = LBY ? angleYaw + (maxDelta / 2.2f) : angleYaw - (maxDelta / 2.2f);
    }
    else if (player->GetVelocity().Length() < 75.77f)
    {
        float LBY = *player->GetLowerBodyYawTarget();
  			player->GetEyeAngles()->y = LBY;
        player->GetAnimState()->goalFeetYaw = (180.f + (angleYaw - feetYaw)) / 360.f;

        Math::NormalizeYaw(player->GetEyeAngles()->y);

        if (feetYaw >= -maxDelta & feetYaw < 0)
  			    player->GetEyeAngles()->y;
        else
  			    -player->GetEyeAngles()->y;

        CUtlVector<AnimationLayer>* layers = player->GetAnimOverlay();

        for (int i = 0; i <= layers->Count(); i++)
        {
            float m_flPlaybackRate = layers->operator[](i).m_flPlaybackRate;

            if (m_flPlaybackRate > 0.1f)
            {
                for (float resolveDelta = 0.0f; resolveDelta < -maxDelta || resolveDelta > maxDelta; resolveDelta = resolveDelta / 2.2f)
                    player->GetEyeAngles()->y = resolveDelta;
            }
        }
    }
}
*/


float yawDistance(float first, float second)
{
    if (first == second)
        return 0.f;

    if ((first <= 0.f && second >= 0.f) || (first >= 0.f && second <= 0.f))
        return std::fabs(first - second); //where the fuck is fabsf?

    if (first > 90.f && second < -90.f) {
        first -= (first - 90.f);
        second += (second + 90.f);
    }
    else if (first < -90.f && second > 90.f) {
        first += (first + 90.f);
        second -= (second - 90.f);
    }

    return std::fabs(first - second);
}

float resLBY(float first, float second)
{
    if (first == second)
        return 0.f;

    if ((first <= 0.f && second >= 0.f) || (first >= 0.f && second <= 0.f))
        return std::fabs(first - second); //where the fuck is fabsf?

    if (first > 35.f && second < -35.f) {
        first -= (first - 35.f);
        second += (second + 35.f);
    }
    else if (first < -35.f && second > 35.f) {
        first += (first + 35.f);
        second -= (second - 35.f);
    }

    return std::fabs(first - second);
}

float legitAA(float first, float second)
{
    if (first == second)
        return 0.f;

    if ((first <= 0.f && second >= 0.f) || (first >= 0.f && second <= 0.f))
        return std::fabs(first - second); //where the fuck is fabsf?

    if (first > 58.f && second < -58.f) {
        first -= (first - 58.f);
        second += (second + 58.f);
    }
    else if (first < -58.f && second > 58.f) {
        first += (first + 58.f);
        second -= (second - 58.f);
    }

    return std::fabs(first - second);
}


float AW(float first, float second)
{
    if (first == second)
        return 0.f;

    if ((first <= 0.f && second >= 0.f) || (first >= 0.f && second <= 0.f))
        return std::fabs(first - second); //where the fuck is fabsf?

    if (first > 116.f && second < -116.f) {
        first -= (first - 116.f);
        second += (second + 116.f);
    }
    else if (first < -116.f && second > 116.f) {
        first += (first + 116.f);
        second -= (second - 116.f);
    }

    return std::fabs(first - second);
}

float Legits(float first, float second)
{
    if (first == second)
        return 0.f;

    if ((first <= 0.f && second >= 0.f) || (first >= 0.f && second <= 0.f))
        return std::fabs(first - second); //where the fuck is fabsf?

    if (first > 5.f && second < -5.f) {
        first -= (first - 5.f);
        second += (second + 5.f);
    }
    else if (first < -5.f && second > 5.f) {
        first += (first + 5.f);
        second -= (second - 5.f);
    }

    return std::fabs(first - second);
}



bool resolveFakeWalk(C_BasePlayer *player)
{
	CUtlVector<AnimationLayer> *layers = player->GetAnimOverlay();
	bool
		bFakewalking = false,
		stage1 = false,			// stages needed cause we are iterating all layers, eitherwise won't work :)
		stage2 = false,
		stage3 = false;


	for (int fw = 0; fw < 3; fw++)
	{
	  if (layers->operator[](fw).m_nSequence == 26 && layers->operator[](fw).m_flWeight < 0.4f)
			stage1 = true;
	  if (layers->operator[](fw).m_nSequence == 7 && layers->operator[](fw).m_flWeight > 0.001f)
			stage2 = true;
	  if (layers->operator[](fw).m_nSequence == 2 && layers->operator[](fw).m_flWeight == 0)
			stage3 = true;
	}

	if (stage1 && stage2)
		if (stage3 || (player->GetFlags() & FL_DUCKING)) // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2
			bFakewalking = true;
		else
			bFakewalking = false;
	else
		bFakewalking = false;

	return bFakewalking;
}

static void printout(C_BasePlayer* player)
{
  CUtlVector<AnimationLayer> *layers = player->GetAnimOverlay();
	IEngineClient::player_info_t playerInfo;
	engine->GetPlayerInfo( player->GetIndex(), &playerInfo );

  float wow = player->GetAnimState()->goalFeetYaw / 2;

  Math::NormalizeYaw(player->GetEyeAngles()->y);



	for (int fw = 0; fw < 3; fw++)
  cvar->ConsoleDPrintf("%s is using GOALFEETYAW %g  on LAYER 7 and %g WEIGHT and %d SEQUENCE.\n", playerInfo.name, player->GetAnimState()->goalFeetYaw, layers->operator[](7).m_flWeight, layers->operator[](fw).m_nSequence);

	for (int fw = 0; fw < 3; fw++)
  cvar->ConsoleDPrintf("%s is using GOALFEETYAW %g  on LAYER 6 and %g WEIGHT and %d SEQUENCE.\n", playerInfo.name, player->GetAnimState()->goalFeetYaw, layers->operator[](6).m_flWeight, layers->operator[](fw).m_nSequence);

//  cvar->ConsoleDPrintf("%s is using GOALFEETYAW %g  on LAYER 5 and %g WEIGHT and %g EYEANGLES.\n", playerInfo.name, player->GetAnimState()->goalFeetYaw, layers->operator[](5).m_flWeight, player->GetEyeAngles()->y);

 // cvar->ConsoleDPrintf("%s is using GOALFEETYAW %g  on LAYER 4 and %g WEIGHT and %g EYEANGLES.\n", playerInfo.name, player->GetAnimState()->goalFeetYaw, layers->operator[](4).m_flWeight, player->GetEyeAngles()->y);



}


//double thanks to Luna for the Resolver skeleton!

static void Resolve(C_BasePlayer* player, float feetYaw, float angleYaw)
{
  if(!engine->IsInGame() || !player)
    return;

	CUtlVector<AnimationLayer> *layers = player->GetAnimOverlay();
	C_BasePlayer* localplayer = ( C_BasePlayer* ) entityList->GetClientEntity( engine->GetLocalPlayer() );

  //still working on this...but here it is for now!
  //going to work on "resolving" legits

  //if (layers->operator[](5).m_flWeight >= 1.f)
   //  player->GetAnimState()->goalFeetYaw = resLBY(player->GetAnimState()->pitch, player->GetAnimState()->eyePitch);

	  if (layers->operator[](6).m_flWeight > 1.f)
      player->GetAnimState()->goalFeetYaw = AW(player->GetAnimState()->pitch, player->GetAnimState()->eyePitch);

	  if (layers->operator[](6).m_flWeight >= 1.f)
      player->GetAnimState()->goalFeetYaw = yawDistance(player->GetAnimState()->pitch, player->GetAnimState()->eyePitch);

	  if (layers->operator[](6).m_flWeight <= 1.f)
      player->GetAnimState()->goalFeetYaw = legitAA(player->GetAnimState()->pitch, player->GetAnimState()->eyePitch);

    if(player->GetVelocity().Length() > 1.0f && player->GetVelocity().Length() <= 110.f)
        resolveFakeWalk(player);

    if(!(player->GetFlags() & FL_ONGROUND))
      player->GetAnimState()->goalFeetYaw = resLBY(player->GetAnimState()->pitch, player->GetAnimState()->eyePitch);


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


      //printout(player);

			if (!Settings::Resolver::resolveAll && std::find(Resolver::Players.begin(), Resolver::Players.end(), entityInformation.xuid) == Resolver::Players.end())
				continue;

			player_data.push_back(std::pair<C_BasePlayer*, QAngle>(player, *player->GetEyeAngles()));

//      float LBY = *player->GetLowerBodyYawTarget();
//      Math::NormalizeAngles(LBY);
//			player->GetEyeAngles()->y = LBY;
			Resolve(player, player->GetAnimState()->currentFeetYaw, player->GetEyeAngles()->y);
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
