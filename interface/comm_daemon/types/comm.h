#ifndef COMM_H
#define COMM_H

#include <stdint.h>

#define BLOCK_LEN 196

#define MAX_REREQ 98

#define FILE_INFO  1
#define FILE_BLOCK 2
#define BLOCK_REQUEST 3

typedef struct file_info {
  uint8_t type;
  uint16_t blocks;
  char filename[32];
} file_info;

typedef struct file_block {
  uint8_t type;
  uint8_t  len;
  uint16_t num;
  uint8_t  data[BLOCK_LEN];
} file_block;

typedef struct block_request {
  uint8_t type;
  uint8_t num;
  uint16_t blocks[MAX_REREQ];
} block_request;

#endif
