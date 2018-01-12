all: bin/serv bin/cli bin/serv_arm bin/cli_arm

bin/serv: serv.c
	gcc serv.c -o bin/serv

bin/cli: cli.c
	gcc cli.c -o bin/cli

bin/serv_arm:
	arm-linux-gnueabi-gcc -DARM serv.c -o bin/serv_arm

bin/cli_arm:
	arm-linux-gnueabi-gcc -DARM cli.c -o bin/cli_arm

clean:
	rm -rf bin/*
