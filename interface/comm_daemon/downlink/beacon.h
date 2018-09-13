#ifndef BEACON_H
#define BEACON_H

#include <stdbool.h>

extern bool beacon_enabled;
volatile extern bool beacon_delay;

void beacon_init();

#endif
