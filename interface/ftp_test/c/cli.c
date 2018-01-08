#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "reqres.h"

#define BUFLEN 512  //Max length of buffer
#define UDP_PORT 8888   //The port on which to send data
 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(int argc, char **argv){

  char *remote_filename = NULL;
  char *local_filename = NULL;
  char *server = "127.0.0.1";
  char * nic = NULL;
  int c;

  opterr = 0;

  while ((c = getopt(argc, argv, "s:r:l:I:")) != -1){
    switch(c){
      case 'r':
        remote_filename = optarg;
        break;
      case 'l':
        local_filename = optarg;
        break;
      case 'I':
        nic = optarg;
        break;
      case 's':
        server = optarg;
        break;
      default:
        break;
    }
  }

  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);
  char buf[BUFLEN];
  char message[BUFLEN];

  if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
    die("socket");
  }

  if(nic != NULL){
    int rc = setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, nic, strlen(nic));
  }

  struct timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    perror("Error");
  }

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(UDP_PORT);

  if(inet_aton(server, &si_other.sin_addr) == 0){
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }
 
  file_request f_req;
  strcpy(f_req.filename, remote_filename);

  if(sendto(s, &f_req, sizeof(file_request) , 0, (struct sockaddr *) &si_other, slen)==-1){
    die("sendto()");
  }

  FILE *requested_file = fopen(local_filename, "w");
  bool first = true;
  block_response b_resp;
  uint64_t count = 0, missing = 0;
  while(true){

    if(recvfrom(s, &b_resp, sizeof(block_response), 0, (struct sockaddr *) &si_other, &slen) == -1){
      printf("Timeout\n");
      break;
    }
    #ifdef ARM
    printf("Received block: num %llu, tot %llu, len %d\n", b_resp.num, b_resp.tot, b_resp.len);
    #else
    printf("Received block: num %lu, tot %lu, len %d\n", b_resp.num, b_resp.tot, b_resp.len);
    #endif

    if(first){
      ftruncate(fileno(requested_file), BLOCK_LEN * b_resp.tot);
      first = false;
    }

    while(b_resp.num > count){
      // TODO add missing blocks to list for rerequest
      count++;
      #ifdef ARM
      printf("Missing block %llu\n", count-1);
      #else
      printf("Missing block %lu\n", count-1);
      #endif
      missing++;
    }

    fseek(requested_file, b_resp.num * BLOCK_LEN, SEEK_SET);
    fwrite(b_resp.data, sizeof(uint8_t), BLOCK_LEN, requested_file);

    if(b_resp.tot == b_resp.num + 1){
      break;
    }

    count++;

  }

  if(missing != 0){
    #ifdef ARM
    printf("Missing %llu blocks\n", missing);
    #else
    printf("Missing %lu blocks\n", missing);
    #endif
  }

  fclose(requested_file);

  close(s);
  return 0;
}
