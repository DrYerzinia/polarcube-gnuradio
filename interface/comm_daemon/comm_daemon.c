#include "downlink/downlink.h"
#include "uplink/uplink.h"

#include <unistd.h>

#define NET_INTERFACE "sl0"

int main(int argc, char **argv){

        li_init(NET_INTERFACE);
        downlink_init(NET_INTERFACE);
        uplink_init(NET_INTERFACE);

        while(1) {
                usleep(1000000);
        }

}
