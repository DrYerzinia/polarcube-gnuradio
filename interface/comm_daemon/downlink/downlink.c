#include "downlink.h"

#include "../li2/li2.h"
#include "beacon.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/inotify.h>

void pack_file_block(uint8_t * buffer, file_block fb){

        buffer[0] = fb.type;
        buffer[1] = fb.len;
        memcpy(buffer+2, &fb.num, 8);
        memcpy(buffer+10,fb.data, BLOCK_LEN);

}

int downlink_fd;
int downlink_wd;

int dl_sock = 0;

struct sockaddr_in gs_addr = {0};
size_t gs_addr_len;

#define GS_DOWNLINK_PORT 35777

useconds_t packet_wait = 40000; // 40ms

#define EVENT_SIZE    (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

void send_file(const char * filename){

        FILE *file_to_send = fopen(filename, "r");
        if(file_to_send == NULL) {
                perror("File not found...\n");
                return;
        }

        fseek(file_to_send, 0L, SEEK_END);
        uint64_t file_blocks = ftell(file_to_send)/BLOCK_LEN+1;
        rewind(file_to_send);

        file_info fi;
        fi.type = FILE_INFO;

        char * fn = strrchr(filename, '/');
        if(fn == NULL) fn = filename;
        else fn += 1;
        strcpy(fi.filename, fn);
        printf("FN2 %s\n", fn);
        fi.file_blocks = file_blocks;
        for(int i = 0; i < 3; i++) {
                printf("Sending file-info\n");
                if(sendto(li_sock, &fi, sizeof(file_info), 0, (struct sockaddr*) &gs_addr, gs_addr_len) == -1) {
                        perror("sendto()");
                        return;
                }
                usleep(1000000);
        }

        uint64_t block_count = 0;
        uint8_t pkt_buff[256];
        while(1) {
                file_block f_block;
                f_block.type = FILE_BLOCK;
                f_block.num = block_count;
                f_block.len = fread(f_block.data, sizeof(uint8_t), BLOCK_LEN, file_to_send);

                if(f_block.len == 0) {
                        printf("Done\n");
                        break;
                }

                #ifdef ARM
                printf("Sending block %llu\n", block_count);
                #else
                printf("Sending block %lu\n", block_count);
                #endif

                float rnum = ((float)rand()/(float)(RAND_MAX));
                pack_file_block(pkt_buff, f_block);
                if(sendto(li_sock, &pkt_buff, BLOCK_LEN+10, 0, (struct sockaddr*) &gs_addr, gs_addr_len) == -1) {
                        perror("sendto()");
                        return;
                }

                usleep(packet_wait);

                block_count++;

        }
}

void * downlink_dir_monitor(void * v) {

        printf("Monitoring for files to downlink.\n");

        char buffer[EVENT_BUF_LEN];

        while(1) {

                printf("Wait noti\n");

                int length = read(downlink_fd, buffer, EVENT_BUF_LEN);
                int i = 0;

                printf("Got noti\n");

                if(length < 0) {
                        perror("read");
                }
                while(i < length) {
                        struct inotify_event *event = (struct inotify_event *) &buffer[i];
                        if(event->len) {
                                if(event->mask & IN_CREATE) {
                                        if(event->mask & IN_ISDIR) {
                                                printf("New directory %s created.\n", event->name);
                                        } else {
                                                uint8_t buf[256];
                                                printf("New file %s created.\n", event->name);
                                                // Encrypt file in /tmp for downlink
                                                sprintf(buf, "openssl enc -aes-256-cbc -in /opt/downlink/%s -out /tmp/%s", event->name, event->name);
                                                system(buf);
                                                //sprintf(buf, "/opt/downlink/%s", event->name);
                                                sprintf(buf, "/tmp/%s", event->name);
                                                beacon_enabled = false;
                                                send_file(buf);
                                                beacon_enabled = true;
                                                remove(buf);
                                        }
                                }
                        }
                        i += EVENT_SIZE + event->len;
                }
        }
}

void downlink_init(){

        memset((char *) &gs_addr, 0, sizeof(gs_addr));
        gs_addr.sin_family = AF_INET;
        gs_addr.sin_port = htons(GS_DOWNLINK_PORT);
        inet_aton("127.0.0.1", &gs_addr.sin_addr);

        gs_addr_len = sizeof(gs_addr);

        printf("Starting downlink thread\n");

        downlink_fd = inotify_init();

        if(downlink_fd < 0) {
                perror("inotify_init");
        }

        downlink_wd = inotify_add_watch(downlink_fd, "/opt/downlink", IN_CREATE);

        pthread_t downlink_dir_monitor_thread;
        if(pthread_create(&downlink_dir_monitor_thread, NULL, downlink_dir_monitor, 0)) {
                fprintf(stderr, "Error creating thread\n");
                return;
        }
}

void downlink_destroy(){
        inotify_rm_watch(downlink_fd, downlink_wd);
        close(downlink_fd);
}
