#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct {
  uint8_t serbaud;
  uint8_t tx_power;
  uint8_t rxrfbaud;
  uint8_t txrfbaud;
  uint8_t rxmod;
  uint8_t txmod;
  uint32_t rxfreq;
  uint32_t txfreq;
  uint8_t source[6];
  uint8_t dest[6];
  uint16_t tx_preamble;
  uint16_t tx_postamble;
  uint16_t function_config;
  uint16_t function_config2;
} lithium_trans_config_t;

enum lithium_serial_bauds {
  SERBAUD_9600 = 0,
  SERBAUD_19200 = 1,
  SERBAUD_38400 = 2,
  SERBAUD_76800 = 3,
  SERBAUD_115200 = 4,
};

enum lithium_rf_bauds {
  RFBAUD_1200 = 0,
  RFBAUD_9600 = 1,
  RFBAUD_19200 = 2,
  RFBAUD_38400 = 3,
  RFBAUD_76800 = 4,
  RFBAUD_115200 = 5,
};

enum lithium_modulation {
  MOD_GMSK = 0,
  MOD_AFSK = 1,
  MOD_BPSK = 2,
};

uint8_t to_rf_baud(int baud){
  switch(baud){
    case 1200:
      return RFBAUD_1200;
    case 9600:
      return RFBAUD_9600;
    case 19200:
      return RFBAUD_19200;
    case 38400:
      return RFBAUD_38400;
    case 76800:
      return RFBAUD_76800;
    case 115200:
      return RFBAUD_115200;
    default:
      return RFBAUD_38400;
  }
}

#define UDP_PORT 35823

int main(int argc, char **argv){

  char * nic = NULL;
  uint8_t uplink_baud = RFBAUD_38400;
  uint8_t downlink_baud = RFBAUD_38400;
  uint8_t tx_power = 138;
  useconds_t packet_wait = 1000; // 0.001s
  int c;

  opterr = 0;

  while ((c = getopt(argc, argv, "I:d:u:t:")) != -1){
    switch(c){
      case 'I':
        nic = optarg;
        break;
      case 'd':
        downlink_baud = to_rf_baud(atoi(optarg));
        break;
      case 'u':
        uplink_baud = to_rf_baud(atoi(optarg));
        break;
      case 't':
        tx_power = atoi(optarg);
        break;
      default:
        break;
    }
  }

  lithium_trans_config_t li_config;
  memcpy(li_config.source, "KD0WGV", 6);
  memcpy(li_config.dest, "ROBERT", 6);
  li_config.function_config = 0x7041;
  li_config.function_config2 = 0x0000;
  li_config.rxfreq = 401000;
  li_config.txfreq = 401000;
  li_config.rxmod = MOD_GMSK;
  li_config.txmod = MOD_GMSK;
  li_config.rxrfbaud = uplink_baud;
  li_config.txrfbaud = downlink_baud;
  li_config.serbaud = SERBAUD_115200;
  li_config.tx_preamble = 19;
  li_config.tx_postamble = 0;
  li_config.tx_power = tx_power;

  // Send
  // SOCKET SETUP
  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);

  if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
    printf("socket");
    return -1;
  }

  if(nic != NULL){
    int rc = setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, nic, strlen(nic));
  }

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(UDP_PORT);

  if(inet_aton("1.1.1.2", &si_other.sin_addr) == 0){
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  //

  if(sendto(s, &li_config, sizeof(lithium_trans_config_t), 0, (struct sockaddr*) &si_other, slen) == -1){
    printf("Failed to send LI config.");
    return -1;
  }

}
