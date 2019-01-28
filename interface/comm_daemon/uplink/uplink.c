#include "uplink.h"

#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

int uplink_sock = 0;
struct sockaddr_in uplink_addr = {0};

void unpack_file_info(uint8_t buffer, file_info * fi){

}

uint16_t count_missing_blocks(uint8_t * r_blocks, uint16_t num_blocks){

        printf("Missing blocks: ");
        uint16_t missing = 0;
        int i;

        for (i = 0; i < num_blocks; i++) {
                if(r_blocks[i] != 1) {
                        printf("%i ", i);
                        fflush(stdout);
                        missing++;
                }
        }
        printf("\n");

        return missing;

}
void receive_file_blocks(uint8_t * r_blocks, FILE * received_file, uint16_t blocks){

        struct sockaddr_in si_other;
        int slen = sizeof(si_other);

        file_block fb;
        while(1) {

                if (recvfrom(uplink_sock, &fb, sizeof(fb), 0, (struct sockaddr *) &si_other, &slen) == -1) {
                        printf("Timeout.\n");
                        break;
                }

				beacon_delay = true;

                if(fb.type != FILE_BLOCK) {
                        continue;
                }

                fseek(received_file, fb.num * BLOCK_LEN, SEEK_SET);
                fwrite(fb.data, sizeof(uint8_t), fb.len, received_file);

                // If we are at last block make file correct length
                if(fb.len < BLOCK_LEN) ftruncate(fileno(received_file), BLOCK_LEN * blocks - (BLOCK_LEN-fb.len));

                printf("rx %d\n", fb.num);
                r_blocks[fb.num] = 1;

        }
}

void * uplink_thread(void * v) {

        struct sockaddr_in si_other;
        int slen = sizeof(si_other);
        int recv_len;

        fprintf(stderr, "Uplink receive loop started\n");

        while(1) {

                file_info fi;
                if ((recv_len = recvfrom(uplink_sock, &fi, sizeof(fi), 0, (struct sockaddr *) &si_other, &slen)) == -1) {
                        printf("Timeout for receiving fileinfo.\n");
                        continue;
                }

				beacon_delay = true;

                if(fi.type == FILE_INFO) {
                        printf("Received fileinfo %s\n", fi.filename);
                } else {
                        continue;
                }

                printf("File blocks %d\n", fi.blocks);

                // TODO change r_blocks to bitarray to save memory
                uint8_t r_blocks[fi.blocks];
                memset(r_blocks, 0, fi.blocks);

                char filename[256];
                sprintf(filename, "/tmp/%s", fi.filename);

                FILE * received_file = fopen(filename, "w");
                ftruncate(fileno(received_file), BLOCK_LEN * fi.blocks);

                printf("Receiving the file blocks...\n");

				uint8_t retry_counter = 0;
                while(1) {

                        receive_file_blocks(r_blocks, received_file, fi.blocks);
                        uint16_t missing = count_missing_blocks(r_blocks, fi.blocks);

                        if(!missing) {

                                fflush(received_file);
                                fclose(received_file);

                                printf("No blocks missing, uplink finished.\n");

                                char command[256];
                                sprintf(command, "openssl aes-256-cbc -d -md sha256 -pass pass:%s -in %s -out /opt/uplink/%s", "imcool", filename, fi.filename);
                                system(command);

                                break;

                        } else {
                                block_request br;
                                br.type = BLOCK_REQUEST;
                                if(missing > MAX_REREQ) {
                                        missing = MAX_REREQ;
                                }
                                br.num = missing;
                                int i, j;
                                for(i = 0, j = 0; i < fi.blocks, j < missing; i++) {
                                        if(r_blocks[i] != 1) {
                                                br.blocks[j++] = i;
                                        }
                                }
                                printf("Requesting %u missing blocks.\n", missing);
								retry_counter++;

                                if(sendto(uplink_sock, &br, sizeof(block_request), 0, (struct sockaddr*) &si_other, slen) == -1) {
                                        fprintf(stderr, "sendto()");
                                }

								// TODO: Validate retry counter
								if(retry_counter > 5){
	                                fclose(received_file);
									break;
								}
                        }
                }
        }

        close(uplink_sock);

}

void uplink_init(const char * nic){

        if ((uplink_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                fprintf(stderr, "socket");
                return;
        }

        if(nic != NULL) {
                int rc = setsockopt(uplink_sock, SOL_SOCKET, SO_BINDTODEVICE, nic, strlen(nic));
        }

        memset((char *) &uplink_addr, 0, sizeof(uplink_addr));

        uplink_addr.sin_family = AF_INET;
        uplink_addr.sin_port = htons(GS_UPLINK_PORT);
        uplink_addr.sin_addr.s_addr = htonl(INADDR_ANY);


        struct timeval tv;
        tv.tv_sec = 7;
        tv.tv_usec = 0;
        if (setsockopt(uplink_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                fprintf(stderr, "Failed to set timeout");
        }

        if(bind(uplink_sock, (struct sockaddr*)&uplink_addr, sizeof(uplink_addr)) == -1) {
                fprintf(stderr, "bind failed");
                return;

        }

        pthread_t uplink_thread_pt;
        if(pthread_create(&uplink_thread_pt, NULL, uplink_thread, 0)) {
                fprintf(stderr, "Error creating thread\n");
                return;
        }
}

void uplink_destroy(){
}
