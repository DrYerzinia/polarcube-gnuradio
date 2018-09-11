all: bin/serv bin/cli bin/serv_arm bin/cli_arm bin/downlink_tx bin/downlink_rx bin/downlink_tx_arm

bin/serv: serv.c
	gcc serv.c -o bin/serv

bin/cli: cli.c
	gcc cli.c -o bin/cli

bin/downlink_tx: downlink_tx.c
	gcc downlink_tx.c -o bin/downlink_tx

bin/downlink_rx: downlink_rx.c
	gcc downlink_rx.c -o bin/downlink_rx

bin/downlink_tx_arm: downlink_tx.c
	arm-linux-gnueabi-gcc -DARM -static downlink_tx.c -o bin/downlink_tx_arm

bin/serv_arm: serv.c
	arm-linux-gnueabi-gcc -DARM -static serv.c -o bin/serv_arm

bin/cli_arm: cli.c
	arm-linux-gnueabi-gcc -DARM -static cli.c -o bin/cli_arm

clean:
	rm -rf bin/*
