#ifndef EXCLUSIVE_EXECUTION_H
#define EXCLUSIVE_EXECUTION_H

// Esecuzione esclusiva tra i task (come takeExclusiveExecution / freeExclusiveExecution
// di ServicesCollector nel progetto principale).
// Qui è un mutex FreeRTOS e non un semplice flag: display e microSD condividono lo stesso
// bus SPI e lo stesso framebuffer, quindi un solo task alla volta può usarli.
// Ogni task racchiude il proprio "giro di lavoro" tra take e free.

void exclusiveExecutionBegin();
void takeExclusiveExecution();
void freeExclusiveExecution();

#endif // EXCLUSIVE_EXECUTION_H
