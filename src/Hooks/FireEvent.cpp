
#include "hooks.h"

#include "../interfaces.h"

typedef bool (*FireEventFn) (void*, IGameEvent*, bool);

bool Hooks::FireEvent(void* thisptr, IGameEvent* event, bool bDontBroadcast)
{
	return gameEventsVMT->GetOriginalMethod<FireEventFn>(9)(thisptr, event, bDontBroadcast);
}
