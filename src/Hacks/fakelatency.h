#ifndef FUZION_FAKELATENCY_H_
#define FUZION_FAKELATENCY_H_

#include <list>
#include "../interfaces.h"

class INetChannel;

namespace FakeLatency {
struct OutgoingDatagram {
  float curtime;
  int32_t inreliablestate;
  int32_t sequencenr;
};

void AddLatencyToNetchan(INetChannel *netchan, float Latency);
void UpdateIncomingSequences();

}  // namespace FakeLatency

#endif  // FUZION_FAKELATENCY_H_
