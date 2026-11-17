#include "RemotePairing.h"

static portMUX_TYPE pairingMux = portMUX_INITIALIZER_UNLOCKED;
static RemotePairing current;
static uint32_t lastGeneration = 0;

void remotePairingSet(const RemotePairing &pairing)
{
  portENTER_CRITICAL(&pairingMux);
  current = pairing;
  current.generation = ++lastGeneration;
  portEXIT_CRITICAL(&pairingMux);
}

void remotePairingClear()
{
  portENTER_CRITICAL(&pairingMux);
  current = RemotePairing();
  current.generation = ++lastGeneration;
  portEXIT_CRITICAL(&pairingMux);
}

bool remotePairingGet(RemotePairing &out)
{
  portENTER_CRITICAL(&pairingMux);
  out = current;
  portEXIT_CRITICAL(&pairingMux);
  return out.valid;
}
