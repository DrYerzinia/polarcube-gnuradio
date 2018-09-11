#include "downlink/downlink.h"

#include <unistd.h>

int main(int argc, char **argv){

        li_init("lo");
        downlink_init();

        while(1) {
                usleep(1000000);
        }

}
