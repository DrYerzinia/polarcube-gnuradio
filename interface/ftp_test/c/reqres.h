#ifndef REQRES_H
#define REQRES_H

#define BLOCK_LEN 128

typedef struct file_request {
  char filename[64];
  uint8_t missing_block_count;
  uint64_t missing_blocks[32];
} file_request;

typedef struct block_response {
  uint64_t num;
  uint64_t tot;
  uint8_t  len;
  uint8_t  data[BLOCK_LEN];
} block_response;

#endif
