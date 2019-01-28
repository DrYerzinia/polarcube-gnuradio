#ifndef UPLINK_H
#define UPLINK_H

#include "../types/comm.h"
#include "../li2/li2.h"
#include "../downlink/beacon.h"

#define GS_UPLINK_PORT 35778

#define MAX_PACKET_SIZE 256

void uplink_init(const char * nic);
void uplink_destroy();

extern struct sockaddr_in uplink_addr;
extern int uplink_sock;

#endif
