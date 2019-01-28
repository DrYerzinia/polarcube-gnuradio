#include "li2.h"

#include <string.h>
#include <stdio.h>

uint8_t to_rf_baud(int baud){
        switch(baud) {
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

lithium_trans_config_t li_config = {0};

struct sockaddr_in li_addr = {0};
size_t li_addr_len;
int li_sock = 0;

uint8_t li_init(const char * nic){

        memcpy(li_config.source, "KD0WGV", 6);
        memcpy(li_config.dest, "ROBERT", 6);
        li_config.function_config = 0x7041;
        li_config.function_config2 = 0x0000;
        li_config.rxfreq = 401000;
        li_config.txfreq = 401000;
        li_config.rxmod = MOD_GMSK;
        li_config.txmod = MOD_GMSK;
        li_config.rxrfbaud = RFBAUD_9600;
        li_config.txrfbaud = RFBAUD_38400;
        li_config.serbaud = SERBAUD_115200;
        li_config.tx_preamble = 3;
        li_config.tx_postamble = 0;
        li_config.tx_power = 138;

        if((li_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                perror("socket");
                return -1;
        }

        if(nic != NULL) {
                int rc = setsockopt(li_sock, SOL_SOCKET, SO_BINDTODEVICE, nic, strlen(nic));
        }

        memset((char *) &li_addr, 0, sizeof(li_addr));
        li_addr.sin_family = AF_INET;
        li_addr.sin_port = htons(LI2_CONFIG_PORT);
        inet_aton("1.1.1.2", &li_addr.sin_addr);

        li_addr_len = sizeof(li_addr);

}

void li2_downlink_baud(uint32_t downlink_baud){
        li_config.txrfbaud = to_rf_baud(downlink_baud);
}

void li2_uplink_baud(uint32_t uplink_baud){
        li_config.rxrfbaud = to_rf_baud(uplink_baud);
}

void li2_tx_power(uint8_t tx_power){
        li_config.tx_power = tx_power;
}

uint8_t li2_configure(){

        if(sendto(li_sock, &li_config, sizeof(lithium_trans_config_t), 0, (struct sockaddr*) &li_addr, li_addr_len) == -1) {
                printf("Failed to send LI config.");
                return -1;
        }

        return 0;
}
