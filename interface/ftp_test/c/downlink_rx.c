#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <limits.h>

#include "reqres.h"

#define BUFLEN 512  //Max length of buffer

void die(char *s)
{
        perror(s);
        exit(1);
}

void unpack_file_block(file_block * fb, uint8_t * buffer){

        fb->type = buffer[0];
        fb->len = buffer[1];
        memcpy(&(fb->num), buffer+2, 8);
        memcpy(&(fb->data), buffer+10, BLOCK_LEN);

}

int main(int argc, char **argv){

        char * nic = NULL;
        int port = 35777;
        int c;

        opterr = 0;

        while ((c = getopt(argc, argv, "I:p:")) != -1) {
                switch(c) {
                case 'I':
                        nic = optarg;
                        break;
                case 'p':
                        port = atoi(optarg);
                        break;
                default:
                        break;
                }
        }

        // START UDP
        struct sockaddr_in si_me, si_other;
        int s, i, slen=sizeof(si_other);
        char buf[BUFLEN];
        char message[BUFLEN];

        if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                die("socket");
        }

        if(nic != NULL) {
                int rc = setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, nic, strlen(nic));
        }

        struct timeval tv;
        tv.tv_sec = 15;
        tv.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                perror("Error");
        }

        si_me.sin_family = AF_INET;
        si_me.sin_port = htons(port);
        si_me.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) == -1) {
                die("bind");
        }
        // END UDP

        file_info fi;
        while(true) {

                if(recvfrom(s, &fi, sizeof(file_info), 0, (struct sockaddr *) &si_other, &slen) == -1) {
                        printf("Waiting for file...\n");
                } else if(fi.type == FILE_INFO) {
                        printf("Got fileinfo %s\n", fi.filename);
                        break;
                }

        }

        FILE *received_file = fopen(fi.filename, "w");
        ftruncate(fileno(received_file), BLOCK_LEN * fi.file_blocks);

        bool first = true;
        file_block f_block;
        uint8_t pkt_buff[256];
        uint64_t last_block = LLONG_MAX, missing = 0, repeat = 0;
        while(true) {

                if(recvfrom(s, &pkt_buff, 256, 0, (struct sockaddr *) &si_other, &slen) == -1) {
                        printf("Timeout\n");
                        break;

                } else if(pkt_buff[0] == FILE_BLOCK) {

                        unpack_file_block(&f_block, pkt_buff);

                        while(f_block.num > last_block + 1) {
                                // TODO add missing blocks to list for rerequestst
        #ifdef ARM
                                printf("Missing block %llu\n", last_block+1);
        #else
                                printf("Missing block %lu\n", last_block+1);
        #endif
                                last_block++;
                                missing++;
                        }

      #ifdef ARM
                        printf("Received block: num %llu, tot %llu, len %d\n", f_block.num, fi.file_blocks, f_block.len);
      #else
                        printf("Received block: num %lu, tot %lu, len %d\n", f_block.num, fi.file_blocks, f_block.len);
      #endif

                        if(f_block.num == last_block) {
        #ifdef ARM
                                printf("Repeat block %llu\n", last_block);
        #else
                                printf("Repeat block %lu\n", last_block);
        #endif
                                repeat++;
                        }

                        last_block = f_block.num;

                        fseek(received_file, f_block.num * BLOCK_LEN, SEEK_SET);
                        fwrite(f_block.data, sizeof(uint8_t), BLOCK_LEN, received_file);

                        if(fi.file_blocks == f_block.num + 1) {
                                break;
                        }


                }
        }

        if(missing != 0) {
      #ifdef ARM
                printf("Missing %llu blocks\n", missing);
      #else
                printf("Missing %lu blocks\n", missing);
      #endif
        }

        if(repeat != 0) {
      #ifdef ARM
                printf("Repeated %llu blocks\n", repeat);
      #else
                printf("Repeated %lu blocks\n", repeat);
      #endif
        }

        fclose(received_file);

        close(s);
        return 0;
}
