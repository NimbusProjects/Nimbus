#include "fakevote.h"
#include "namechanger.h"

#include "../Utils/xorstring.h"
#include "../settings.h"
#include "../interfaces.h"

char Settings::FakeVote::message[128] = "Call a timeout?";
char Settings::FakeVote::cmd[128] = "swapteams";

bool fakevote_enabled = false;
int fakevote_id = -1;
int fakevote_step = 0;
int fakevote_type = 0;
std::string oname = "invalid";

std::string GetLocalName()
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer) return std::string("unconnected");

	IEngineClient::player_info_t playerInfo;
	engine->GetPlayerInfo(engine->GetLocalPlayer(), &playerInfo);
	return std::string(playerInfo.name);
}

void CallVotekick(int userid)
{
	std::string cmd = "callvote Kick " + std::to_string(userid);
	engine->ClientCmd_Unrestricted(cmd.c_str());
}

void CallCustomVote(char* vote)
{
	std::string cmd = "callvote ";
	cmd += vote;
	engine->ClientCmd_Unrestricted(cmd.c_str());
}

void CallSpoofname()
{
	std::string name = "";
	name.insert(name.begin(), 30, '\n');
	name += Settings::FakeVote::message;
	name.insert(name.end(), 180, '\n');
	NameChanger::SetName(name.c_str());
}

void FakeVote::CallVote(int type, int userid)
{
	if (fakevote_enabled)
		return;

	oname = GetLocalName();
	NameChanger::SetName(XORSTR("\n\xAD\xAD\xAD"));
	fakevote_step = 0;
	fakevote_id = userid;
	fakevote_type = type;
	fakevote_enabled = true;
	return;
}

void FakeVote::CreateMove(CUserCmd* cmd)
{
	if (!fakevote_enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	float currentTime = globalVars->curtime;
	static float timeStamp = currentTime;

	if (currentTime - timeStamp < 1)
		return;

	timeStamp = currentTime;

	switch (fakevote_step)
	{
		case 0:
		{
			fakevote_step++;
			NameChanger::SetName(XORSTR("\n\xAD\xAD\xAD")); // re-glitch just to be sure.
			break;
		}
		case 1:
		{
			fakevote_step++;
			CallSpoofname();
			break;
		}
		case 2:
		{
			fakevote_step++;
			if (fakevote_type == 0)
				CallVotekick(fakevote_id);
			else
				CallCustomVote(Settings::FakeVote::cmd);
			break;
		}
		case 3:
		{
			fakevote_step++;
			NameChanger::SetName(oname.c_str());
			break;
		}
		case 4:
		{
			fakevote_step = 0;
			fakevote_id = -1;
			fakevote_enabled = false;
			break;
		}
		default: // very impossible
		{
			fakevote_step = 0;
			fakevote_id = -1;
			fakevote_enabled = false;
			cvar->ConsoleDPrintf("[FAKEVOTE] switch() statment returned default. Fake Vote cancelled.\n");
		}
	}
}

/*void FakeVote::FireGameEvent(IGameEvent* event)
{
	if (!event)
		return;
}*/
