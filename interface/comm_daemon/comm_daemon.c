#include "downlink/downlink.h"
#include "uplink/uplink.h"

#include <unistd.h>

int main(int argc, char **argv){

        li_init("sl0");

        downlink_init("sl0");
        uplink_init("sl0");

		beacon_init();

        while(1) {
                usleep(1000000);
        }

}
