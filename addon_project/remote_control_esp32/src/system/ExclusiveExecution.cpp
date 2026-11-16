#include "ExclusiveExecution.h"

#include <Arduino.h>

static SemaphoreHandle_t exclusiveMutex = nullptr;

void exclusiveExecutionBegin()
{
  if (exclusiveMutex == nullptr)
  {
    // mutex (non semaforo binario): ha la priority inheritance, così un task a bassa
    // priorità che tiene il lock non viene scavalcato a tempo indeterminato
    exclusiveMutex = xSemaphoreCreateMutex();
  }
}

void takeExclusiveExecution()
{
  if (exclusiveMutex != nullptr)
  {
    xSemaphoreTake(exclusiveMutex, portMAX_DELAY);
  }
}

void freeExclusiveExecution()
{
  if (exclusiveMutex != nullptr)
  {
    xSemaphoreGive(exclusiveMutex);
  }
}
