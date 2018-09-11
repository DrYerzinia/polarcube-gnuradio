#ifndef DOWNLINK_H
#define DOWNLINK_H

#include "../types/comm.h"
#include "../li2/li2.h"

#define GS_DOWNLINK_PORT 35777

#define FILE_INFO_REPEATS 3

void downlink_init(const char * nic);
void downlink_destroy();

extern struct sockaddr_in gs_addr;
extern struct sockaddr_in my_addr;
extern int downlink_sock;

#endif
