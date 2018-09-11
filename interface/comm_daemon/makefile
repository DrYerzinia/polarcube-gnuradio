all: create_dirs bin/comm_daemon

create_dirs:
	mkdir -p bin
	mkdir -p build/obj

bin/comm_daemon: build/obj/comm_daemon.o build/obj/downlink.o build/obj/li2.o build/obj/beacon.o
	gcc -o $@ build/obj/comm_daemon.o build/obj/downlink.o build/obj/li2.o build/obj/beacon.o -lpthread

build/obj/comm_daemon.o: comm_daemon.c
	gcc -o $@ -c comm_daemon.c

build/obj/downlink.o: downlink/downlink.c
	gcc -o $@ -c downlink/downlink.c

build/obj/li2.o: li2/li2.c
	gcc -o $@ -c li2/li2.c

build/obj/beacon.o: downlink/beacon.o
	gcc -o $@ -c downlink/beacon.c

clean:
	rm -rf bin
	rm -rf build
