#ifndef DOWNLINK_H
#define DOWNLINK_H

#include "../types/comm.h"
#include "../li2/li2.h"

void downlink_init();

extern struct sockaddr_in gs_addr;
extern size_t gs_addr_len;

#endif
