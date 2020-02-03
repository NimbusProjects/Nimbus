
#include "fakelatency.h"
#include "../SDK/definitions.h"
#include "../SDK/CBaseClientState.h"
#include "../SDK/INetChannel.h"
#include "../Utils/xorstring.h"
#include "../Hooks/hooks.h"
#include "../settings.h"

float Settings::FakeLatency::value = 1;
//float Latency = Settings::FakeLatency::value;
static std::list<FakeLatency::OutgoingDatagram> sequences;
static int32_t lastincomingsequencenumber = Settings::FakeLatency::value;
static INetChannel* netchannel_global = nullptr;


void FakeLatency::AddLatencyToNetchan(INetChannel* netchan, float Latency) {
  for (auto& seq : sequences) {
    if (globalVars->realtime - seq.curtime >= Latency) {
      netchan->m_nInReliableState = seq.inreliablestate;
      netchan->m_nInSequenceNr = seq.sequencenr;
      break;
    }
  }
}

void FakeLatency::UpdateIncomingSequences() {
  return;

  const auto netchannel = GetLocalClient(-1)->m_NetChannel;
  static bool already_hooked = false;

  if (netchannel) {
    if (!already_hooked) {
      already_hooked = true;
      netChannelVMT = new VMT(netchannel);
      netChannelVMT->HookVM(Hooks::SendDatagram, 46);
      netChannelVMT->ApplyVMT();
      cvar->ConsoleColorPrintf(ColorRGBA(255, 127, 127, 255),
                               "senddatagram hooked\n");
      return;
    }

    C_BasePlayer* localplayer =
        (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive()) {
      if (!sequences.empty()) sequences.clear();
      return;
    }

    if (netchannel->m_nInSequenceNr > lastincomingsequencenumber) {
      lastincomingsequencenumber = netchannel->m_nInSequenceNr;

      sequences.push_front(OutgoingDatagram{globalVars->realtime,
                                            netchannel->m_nInReliableState,
                                            netchannel->m_nInSequenceNr});
    }

    if (sequences.size() > 2048) sequences.pop_back();
  }
}
