#include "hooks.h"

#include "../interfaces.h"
#include "../settings.h"

#include "../Hacks/svcheats.h"

typedef bool (*comp_minispec) (void*);
//comp_minispec oCompMiniSpec;
//ConVar* minispec = cvar->FindVar("sv_competitive_minispec");

float Hooks::compMinispec(void* thisptr)
{
/*
	bool oCompMiniSpec = clientModeVMT->GetOriginalMethod<comp_minispec>(13)(thisptr);

  minispec->fnChangeCallback = 0;
	minispec->SetValue(0);

	return 0;
  */
}

