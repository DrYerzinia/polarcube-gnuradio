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
#include <sys/stat.h>
#include <fcntl.h>

int downlink_fd;
int downlink_wd;

int downlink_sock;

struct sockaddr_in gs_addr = {0};
struct sockaddr_in beacon_addr = {0};
struct sockaddr_in my_addr = {0};

useconds_t packet_wait = 30000; // 30ms

#define EVENT_SIZE    (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

void pack_file_block(uint8_t * buffer, file_block fb){

        buffer[0] = fb.type;
        buffer[1] = fb.len;
        memcpy(buffer+2, &fb.num, 2);
        memcpy(buffer+4,fb.data, BLOCK_LEN);

}

void enable_flow_control(){

        printf("Exporting PC26 GPIO90 ACK for Flow Control\n");

        int fd;

        // Export flow contol GPIO in sysfs
        fd = open("/sys/class/gpio/export", O_WRONLY);
        if (-1 == fd) {
                fprintf(stderr, "Failed to open export for writing!\n");
                return;
        }
        write(fd, "90", 2);
        close(fd);

        // Set flow contron GPIO as input
        fd = open("/sys/class/gpio/pioC26/direction", O_WRONLY);
        if(-1 == fd) {
                fprintf(stderr, "Failed to open gpio direction for writing!\n");
                return;
        }
        if(-1 == write(fd, "in", 2)) {
                fprintf(stderr, "Failed to set direction!\n");
                return;
        }
        close(fd);

}

bool is_com_buffer_almost_full(){

        char value_str[3];
        int fd;

        fd = open("/sys/class/gpio/pioC26/value", O_RDONLY);
        if (-1 == fd) {
                fprintf(stderr, "Failed to open gpio value for reading!\n");
                return false;
        }

        if (-1 == read(fd, value_str, 3)) {
                fprintf(stderr, "Failed to read value!\n");
                return false;
        }

        close(fd);

        int state = atoi(value_str);

        if(state == 1) return false;
        return true;

}

void send_file(const char * filename){

        int i;
        uint16_t block_count = 0;
        uint8_t pkt_buff[256];

        struct sockaddr_in si_other;
        int slen = sizeof(si_other);

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

        const char * fn = strrchr(filename, '/');
        if(fn == NULL) fn = filename;
        else fn += 1;
        strcpy(fi.filename, fn);
        fi.blocks = file_blocks;
        for(i = 0; i < FILE_INFO_REPEATS; i++) {
                printf("Sending file-info size %u\n", sizeof(file_info));
                if(sendto(downlink_sock, &fi, sizeof(file_info), 0, (struct sockaddr*) &gs_addr, sizeof(gs_addr)) == -1) {
                        perror("sendto()");
                        return;
                }
                usleep(500000);
        }

        while(1) {
                file_block f_block;
                f_block.type = FILE_BLOCK;
                f_block.num = block_count;
                f_block.len = fread(f_block.data, sizeof(uint8_t), BLOCK_LEN, file_to_send);

                if(f_block.len == 0) {
                        printf("Sent\n");
                        break;
                }

                printf("Sending block %u\n", f_block.num);

                pack_file_block(pkt_buff, f_block);
                if(sendto(downlink_sock, &pkt_buff, BLOCK_LEN+4, 0, (struct sockaddr*) &gs_addr, sizeof(gs_addr)) == -1) {
                        perror("sendto()");
                        return;
                }

                usleep(packet_wait);
                while(is_com_buffer_almost_full()) usleep(packet_wait*4);

                block_count++;

                // Ignore uplined packets
                fd_set readfds;
                FD_ZERO(&readfds);
                FD_SET(downlink_sock, &readfds);
                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 0;
                while(select(downlink_sock+1, &readfds, NULL, NULL, &tv) == 1) {
                        recvfrom(downlink_sock, &pkt_buff, sizeof(256), 0, (struct sockaddr *) &si_other, &slen);
                        printf("Deleting uplink packet!\n");
                }

        }

        // TODO 10 second wait for retransmission requests or break on new file info

        block_request br = {0};

        int retry_counter = 0;
        while(1) {
                if(recvfrom(downlink_sock, &br, sizeof(br), 0, (struct sockaddr *) &si_other, &slen) == -1) {
                        printf("Timeout. retry: %u\n", retry_counter);
                        if(retry_counter >= 4) {
                                break;
                        }
                        retry_counter++;
                        br.type = 0;
                }
                if(br.type == BLOCK_REQUEST) {
                        printf("Missing blocks requested\n");

                        retry_counter = 0;

                        for(i = 0; i < br.num; i++) {

                                file_block f_block;
                                f_block.type = FILE_BLOCK;
                                f_block.num = br.blocks[i];
                                fseek(file_to_send, br.blocks[i] * BLOCK_LEN, SEEK_SET);
                                f_block.len = fread(f_block.data, sizeof(uint8_t), BLOCK_LEN, file_to_send);

                                printf("Sending block %u\n", f_block.num);

                                pack_file_block(pkt_buff, f_block);

                                if(sendto(downlink_sock, &pkt_buff, BLOCK_LEN+4, 0, (struct sockaddr*) &gs_addr, sizeof(gs_addr)) == -1) {
                                        perror("sendto()");
                                        return;
                                }

                                usleep(packet_wait);
                                while(is_com_buffer_almost_full()) usleep(packet_wait*4);

                        }

                } else if(br.type == DL_DONE) {
                        printf("DL Done\n");
                        break;
                } else {
                        printf("Unexpected Type: %u", br.type);
                }
        }

        fclose(file_to_send);

        printf("Done\n");

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
                                                sprintf(buf, "openssl aes-256-cbc -e -md sha256 -pass pass:%s -in /opt/downlink/%s -out /tmp/%s", "imcool", event->name, event->name);
                                                system(buf);
                                                sprintf(buf, "/tmp/%s", event->name);
                                                beacon_enabled = false;
                                                send_file(buf);
                                                beacon_enabled = true;
                                                remove(buf);
                                                printf("Delete: %s\n", buf);
                                                sprintf(buf, "/opt/downlink/%s", event->name);
                                                remove(buf);
                                                printf("Delete: %s\n", buf);
                                        }
                                }
                        }
                        i += EVENT_SIZE + event->len;
                }
        }
}

void downlink_init(const char * nic){

        if((downlink_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                fprintf(stderr, "socket");
                return;
        }

        if(nic != NULL) {
                int rc = setsockopt(downlink_sock, SOL_SOCKET, SO_BINDTODEVICE, nic, strlen(nic));
        }

        memset((char *) &gs_addr, 0, sizeof(gs_addr));
        gs_addr.sin_family = AF_INET;
        gs_addr.sin_port = htons(GS_DOWNLINK_PORT);
        inet_aton("1.1.1.2", &gs_addr.sin_addr);

        memset((char *) &beacon_addr, 0, sizeof(beacon_addr));
        beacon_addr.sin_family = AF_INET;
        beacon_addr.sin_port = htons(35770);
        inet_aton("1.1.1.2", &beacon_addr.sin_addr);

        memset((char *) &my_addr, 0, sizeof(my_addr));
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(GS_DOWNLINK_PORT);
        inet_aton("1.1.1.1", &my_addr.sin_addr);

        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        if (setsockopt(downlink_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                fprintf(stderr, "Failed to set dl timeout\n");
        }

        if(bind(downlink_sock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1) {
                fprintf(stderr, "dl bind failed\n");
                return;

        }

        enable_flow_control();

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
