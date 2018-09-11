#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "reqres.h"

#define BUFLEN 512
#define UDP_PORT 35777

void die(char *s){

        perror(s);
        exit(1);

}

void pack_file_block(uint8_t * buffer, file_block fb){

        buffer[0] = fb.type;
        buffer[1] = fb.len;
        memcpy(buffer+2, &fb.num, 8);
        memcpy(buffer+10,fb.data, BLOCK_LEN);

}

int main(int argc, char **argv){

        char * nic = NULL;
        char * server = NULL;
        char * filename = NULL;
        int port = 8888;
        useconds_t packet_wait = 1000; // 0.001s
        int c;

        opterr = 0;

        while ((c = getopt(argc, argv, "I:w:f:s:p:")) != -1) {
                switch(c) {
                case 'I':
                        nic = optarg;
                        break;
                case 'w':
                        packet_wait = atoi(optarg);
                        break;
                case 'f':
                        filename = optarg;
                        break;
                case 's':
                        server = optarg;
                        break;
                case 'p':
                        port = atoi(optarg);
                        break;
                default:
                        break;
                }
        }

        // SOCKET SETUP
        struct sockaddr_in si_other;
        int s, i, slen=sizeof(si_other);
        char buf[BUFLEN];
        char message[BUFLEN];

        if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                die("socket");
        }

        if(nic != NULL) {
                int rc = setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, nic, strlen(nic));
        }

        memset((char *) &si_other, 0, sizeof(si_other));
        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(port);

        if(inet_aton(server, &si_other.sin_addr) == 0) {
                fprintf(stderr, "inet_aton() failed\n");
                printf("%s\n",server);
                exit(1);
        }
        //

        FILE *file_to_send = fopen(filename, "r");
        if(file_to_send == NULL) {
                die("File not found...\n");
        }

        fseek(file_to_send, 0L, SEEK_END);
        uint64_t file_blocks = ftell(file_to_send)/BLOCK_LEN+1;
        rewind(file_to_send);

        file_info fi;
        fi.type = FILE_INFO;
        strcpy(fi.filename, filename);
        fi.file_blocks = file_blocks;
        for(int i = 0; i < 3; i++) {
                printf("Sending file-info\n");
                if(sendto(s, &fi, sizeof(file_info), 0, (struct sockaddr*) &si_other, slen) == -1) {
                        die("sendto()");
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
                if(sendto(s, &pkt_buff, BLOCK_LEN+10, 0, (struct sockaddr*) &si_other, slen) == -1) {
                        die("sendto()");
                }

                usleep(packet_wait);

                block_count++;

        }

        close(s);
        return 0;
}
