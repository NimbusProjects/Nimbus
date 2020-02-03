#include "hooks.h"

#include "../Hacks/fakelatency.h"
#include "../interfaces.h"

bool enabled = false;

static constexpr float FAKE_LATENCY_AMOUNT = 0.2f;

typedef int (*SendDatagramFn)(INetChannel*, void*);

#define NET_FRAMES_BACKUP 64  // must be power of 2
#define NET_FRAMES_MASK (NET_FRAMES_BACKUP - 1)

int Hooks::SendDatagram(INetChannel* netchan, void* datagram) {
  cvar->ConsoleColorPrintf(ColorRGBA(255, 127, 127, 255),
                           "senddatagram called\n");
  if (!enabled) {
    return netChannelVMT->GetOriginalMethod<SendDatagramFn>(46)(netchan,
                                                                datagram);
  }

  int instate = netchan->m_nInReliableState;
  int insequencenr = netchan->m_nInSequenceNr;

  FakeLatency::AddLatencyToNetchan(netchan, FAKE_LATENCY_AMOUNT);

  int ret =
      netChannelVMT->GetOriginalMethod<SendDatagramFn>(46)(netchan, datagram);

  cvar->ConsoleColorPrintf(ColorRGBA(255, 0, 0, 255), "xd diff: %d\n",
                           insequencenr - netchan->m_nInSequenceNr);
  netchan->m_nInReliableState = instate;
  netchan->m_nInSequenceNr = insequencenr;

  return ret;
}
