#include "svcheats.h"

#include "../interfaces.h"
#include "../settings.h"

bool Settings::SvCheats::enabled = false;
bool Settings::SvCheats::gravity::enabled = false;
int Settings::SvCheats::gravity::amount = 800;
bool Settings::SvCheats::viewmodel::enabled = false;
float Settings::SvCheats::viewmodel::fov = 68;
float Settings::SvCheats::viewmodel::x = 1;
float Settings::SvCheats::viewmodel::y = 1;
float Settings::SvCheats::viewmodel::z = 1;

void SvCheats::MiniSpec(const char* sv_competitive_minispec)
{
/*
	ConVar* minispec = cvar->FindVar("sv_competitive_minispec");
  minispec->fnChangeCallback = 0;
  minispec->flags &= ~FCVAR_CHEAT;
	minispec->SetValue(sv_competitive_minispec);
  return;
*/
}

void SvCheats::FrameStageNotify(ClientFrameStage_t stage)
{
	if (!Settings::SvCheats::enabled)
		return;

	if (!engine->IsInGame())
        return;

//  if(Settings::SvCheats::enabled)
//    SvCheats::MiniSpec(0);


  //if(Settings::SvCheats::nospread::enabled){
 // }

  if(Settings::SvCheats::gravity::enabled){
	  static ConVar* invertedragdoll = cvar->FindVar("cl_ragdoll_gravity");
	  invertedragdoll->flags &= ~FCVAR_CHEAT;
	  invertedragdoll->SetValue(Settings::SvCheats::gravity::amount);
  }else{
    static ConVar* invertedragdoll = cvar->FindVar("cl_ragdoll_gravity");
	  invertedragdoll->flags &= ~FCVAR_CHEAT;
	  invertedragdoll->SetValue(800);
  }
  if(Settings::SvCheats::viewmodel::enabled){
	  static ConVar* viewmodelfov = cvar->FindVar("viewmodel_fov");
	  static ConVar* viewmodelx = cvar->FindVar("viewmodel_offset_x");
	  static ConVar* viewmodely = cvar->FindVar("viewmodel_offset_y");
	  static ConVar* viewmodelz = cvar->FindVar("viewmodel_offset_z");
    static ConVar* minispec = cvar->FindVar("sv_cheats");



    minispec->flags &= ~FCVAR_CHEAT;
    viewmodelfov->flags &= ~FCVAR_CHEAT;
	  viewmodelx->flags &= ~FCVAR_CHEAT;
	  viewmodely->flags &= ~FCVAR_CHEAT;
	  viewmodelz->flags &= ~FCVAR_CHEAT;

    viewmodelfov->SetValue(Settings::SvCheats::viewmodel::fov);
    viewmodelx->SetValue(Settings::SvCheats::viewmodel::x);
    viewmodely->SetValue(Settings::SvCheats::viewmodel::y);
    viewmodelz->SetValue(Settings::SvCheats::viewmodel::z);
    minispec->SetValue(1);
  }



}
