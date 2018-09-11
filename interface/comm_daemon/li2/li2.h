#ifndef LI2_H
#define LI2_H

#define LI2_CONFIG_PORT 35823

#include <stdint.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

extern struct sockaddr_in li_addr;
extern size_t li_addr_len;
extern int li_sock;

uint8_t li_init(const char * nic);

#endif
