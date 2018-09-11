#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "reqres.h"

#define BUFLEN 512
#define UDP_PORT 8888

void die(char *s){

  perror(s);
  exit(1);

}

void setup_udp(int *s, struct sockaddr_in *si_me, char * nic){

  if((*s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
    die("socket");
  }

  if(nic != NULL){
    int rc = setsockopt(*s, SOL_SOCKET, SO_BINDTODEVICE, nic, strlen(nic));
  }

  memset((char *) si_me, 0, sizeof(si_me));
     
  si_me->sin_family = AF_INET;
  si_me->sin_port = htons(UDP_PORT);
  si_me->sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(*s, (struct sockaddr *) si_me, sizeof(*si_me)) == -1){
    die("bind");
  }

}

int main(int argc, char **argv){

  char * drop_packets = NULL;
  char * nic = NULL;
  useconds_t packet_wait = 1000; // 0.001s
  int c;

  opterr = 0;

  while ((c = getopt(argc, argv, "d:I:w:")) != -1){
    switch(c){
      case 'd':
        drop_packets = optarg;
        break;
      case 'I':
        nic = optarg;
        break;
      case 'w':
        packet_wait = atoi(optarg);
      default:
        break;
    }
  }

  float packet_loss = 0.0f;
  if(drop_packets != NULL){
    packet_loss = atof(drop_packets);
  }

  struct sockaddr_in si_me, si_other;
  int s, slen = sizeof(si_other), recv_len;

  setup_udp(&s, &si_me, nic);

  while(1){

    printf("Waiting for request...\n");
    fflush(stdout);

    char buf[BUFLEN];
    if((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1){
      die("recvfrom()");
    }

    struct file_request * f_req;
    f_req = (file_request*)&buf;
    printf("File requested %s, missing_blocks %d\n", f_req->filename, f_req->missing_block_count);

    usleep(500000); // 0.5s

    FILE *requested_file = fopen(f_req->filename, "r");
    if(requested_file == NULL){
      printf("File not found...\n");
      continue;
    }

    fseek(requested_file, 0L, SEEK_END);
    uint64_t file_blocks = ftell(requested_file)/128+1;
    rewind(requested_file);

    uint64_t block_count = 0;
    while(1){
      struct block_response b_resp;
      b_resp.num = block_count;
      b_resp.tot = file_blocks;
      b_resp.len = fread(b_resp.data, sizeof(uint8_t), BLOCK_LEN, requested_file);

      if(b_resp.len == 0){
        printf("Done\n");
        break;
      }

      #ifdef ARM
      printf("Sending block %llu\n", block_count);
      #else
      printf("Sending block %lu\n", block_count);
      #endif

      float rnum = ((float)rand()/(float)(RAND_MAX));
      if(rnum > packet_loss){
        if(sendto(s, &b_resp, sizeof(block_response), 0, (struct sockaddr*) &si_other, slen) == -1){
          die("sendto()");
        }
      } else {
        printf("Dropped\n");
      }
 
      usleep(packet_wait);

      block_count++;

    }

  }
 
  close(s);
  return 0;
}

