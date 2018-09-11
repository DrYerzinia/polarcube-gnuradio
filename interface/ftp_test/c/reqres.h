#ifndef REQRES_H
#define REQRES_H

#define BLOCK_LEN 196

typedef struct file_request {
  char filename[64];
  uint8_t missing_block_count;
  uint64_t missing_blocks[16];
} file_request;

typedef struct block_response {
  uint64_t num;
  uint64_t tot;
  uint8_t  len;
  uint8_t  data[BLOCK_LEN];
} block_response;

#define FILE_INFO  1
#define FILE_BLOCK 2

typedef struct file_info {
  uint8_t type;
  char filename[32];
  uint64_t file_blocks;
  uint8_t sha256sum[32];
} file_info;

typedef struct file_block {
  uint8_t type;
  uint8_t  len;
  uint64_t num;
  uint8_t  data[BLOCK_LEN];
} file_block;

#endif
