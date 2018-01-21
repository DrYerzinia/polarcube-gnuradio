all: bin/li_control bin/li_control_arm

bin/li_control: li_control.c
	gcc li_control.c -o bin/li_control

bin/li_control_arm: li_control.c
	arm-linux-gnueabi-gcc -DARM -static li_control.c -o bin/li_control_arm

clean:
	rm -rf bin/*
