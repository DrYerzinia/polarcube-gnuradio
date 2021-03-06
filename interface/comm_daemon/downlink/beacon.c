#include "beacon.h"

#include "downlink.h"
#include "../li2/li2.h"

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

bool beacon_enabled = true;
volatile bool beacon_delay = false;

#define MAX_BEACON_SIZE 200

void * beacon(void * v) {

        printf("Beacon thread started\n");

        while(1) {

                usleep(10000000); // 10s

				// During uplink reception of data causes 10s beacon delay so we dont interrupt
				while(beacon_delay) {
					beacon_delay = false;
	                usleep(10000000); // 10s
				}

                if(beacon_enabled) {

                        FILE *beacon_file = fopen("/opt/beacon/beacon", "r");
                        if(beacon_file == NULL) {
                                perror("Beacon not found...\n");
                                continue;
                        }

                        uint8_t beacon_buf[MAX_BEACON_SIZE+1];
                        beacon_buf[0] = BEACON;
                        size_t read = fread(beacon_buf+1, sizeof(uint8_t), MAX_BEACON_SIZE, beacon_file);

						printf("Sending Beacon\n");
						if(sendto(downlink_sock, &beacon_buf, read+1, 0, (struct sockaddr*) &beacon_addr, sizeof(beacon_addr)) == -1){
                                perror("sendto()");
                                continue;
                        }
                }
        }

}

void beacon_init(){

        pthread_t beacon_thread;
        if(pthread_create(&beacon_thread, NULL, beacon, 0)) {
                fprintf(stderr, "Error creating thread\n");
                return;
        }

}
