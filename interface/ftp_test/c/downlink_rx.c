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

  char * nic = NULL;
  int c;

  opterr = 0;

  while ((c = getopt(argc, argv, "I:")) != -1){
    switch(c){
      case 'I':
        nic = optarg;
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

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(UDP_PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) == -1){
    die("bind");
  }
  // END UDP

  file_info fi;
  while(true){

    if(recvfrom(s, &fi, sizeof(file_info), 0, (struct sockaddr *) &si_other, &slen) == -1){
      printf("Waiting for file...\n");  
    } else if(fi.type == FILE_INFO){
      printf("Got fileinfo\n");
      break;
    }

  }

  FILE *received_file = fopen(fi.filename, "w");
  ftruncate(fileno(received_file), BLOCK_LEN * fi.file_blocks);

  bool first = true;
  file_block f_block;
  uint64_t count = 0, missing = 0;
  while(true){

    if(recvfrom(s, &f_block, sizeof(file_block), 0, (struct sockaddr *) &si_other, &slen) == -1){
      printf("Timeout\n");
      break;
 
   } else if(f_block.type == FILE_BLOCK){

      #ifdef ARM
      printf("Received block: num %llu, tot %llu, len %d\n", f_block.num, fi.file_blocks, f_block.len);
      #else
      printf("Received block: num %lu, tot %lu, len %d\n", f_block.num, fi.file_blocks, f_block.len);
      #endif

      while(f_block.num > count){
        // TODO add missing blocks to list for rerequest
        count++;
        #ifdef ARM
        printf("Missing block %llu\n", count);
        #else
        printf("Missing block %lu\n", count);
        #endif

        missing++;
      }

      fseek(received_file, f_block.num * BLOCK_LEN, SEEK_SET);
      fwrite(f_block.data, sizeof(uint8_t), BLOCK_LEN, received_file);

      if(fi.file_blocks == f_block.num + 1){
        break;
      }

      count++;


    }
  }

  if(missing != 0){
      #ifdef ARM
        printf("Missing %llu blocks\n", missing);
      #else
        printf("Missing %lu blocks\n", missing);
      #endif
  } 

  fclose(received_file);

  close(s);
  return 0;
}
