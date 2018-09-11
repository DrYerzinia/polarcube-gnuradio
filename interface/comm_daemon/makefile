CARMFLAGS=--sysroot=/opt/sysroot/

all: create_dirs bin/comm_daemon bin/comm_daemon_arm

create_dirs:
	mkdir -p bin
	mkdir -p build/obj

bin/comm_daemon: build/obj/comm_daemon.o build/obj/downlink.o build/obj/li2.o build/obj/beacon.o build/obj/uplink.o
	gcc -o $@ $? -lpthread

build/obj/comm_daemon.o: comm_daemon.c
	gcc -o $@ -c $?

build/obj/downlink.o: downlink/downlink.c
	gcc -o $@ -c $?

build/obj/uplink.o: uplink/uplink.c
	gcc -o $@ -c $?

build/obj/li2.o: li2/li2.c
	gcc -o $@ -c $?

build/obj/beacon.o: downlink/beacon.c
	gcc -o $@ -c $?

bin/comm_daemon_arm: build/obj/comm_daemon_arm.o build/obj/downlink_arm.o build/obj/li2_arm.o build/obj/beacon_arm.o build/obj/uplink_arm.o
	arm-linux-gnueabi-gcc $(CARMFLAGS) -o $@ $? -lpthread

build/obj/comm_daemon_arm.o: comm_daemon.c
	arm-linux-gnueabi-gcc $(CARMFLAGS) -o $@ -c $?

build/obj/downlink_arm.o: downlink/downlink.c
	arm-linux-gnueabi-gcc $(CARMFLAGS) -o $@ -c $?

build/obj/uplink_arm.o: uplink/uplink.c
	arm-linux-gnueabi-gcc $(CARMFLAGS) -o $@ -c $?

build/obj/li2_arm.o: li2/li2.c
	arm-linux-gnueabi-gcc $(CARMFLAGS) -o $@ -c $?

build/obj/beacon_arm.o: downlink/beacon.c
	arm-linux-gnueabi-gcc $(CARMFLAGS) -o $@ -c $?

clean:
	rm -rf bin
	rm -rf build
