CC=gcc
#CC=/opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-fsl-linux-gnueabi-gcc
COMMON=src/common/hash.o src/common/proto.o src/common/signal.o
LDFLAGS=-lpthread
#`pkg-config --libs /home/opc/Kombain/libmodbus-3.0.6/libmodbus.pc`
CFLAGS+=-g -Isrc
#-DMODBUS_ENABLE `pkg-config --cflags /home/opc/Kombain/libmodbus-3.0.6/libmodbus.pc`

PROGRAMS=signalrouter client_virtual client_modbus

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $^

all: signalrouter client_virtual client_modbus

signalrouter: $(COMMON) src/server/signalrouter.o src/server/servercommand.o src/common/subscription.o src/server/serverevents.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

client_virtual: $(COMMON) src/client/client.o src/client/clientcommand.o src/virtualclient.o src/common/ringbuffer.o src/client/signalhelper.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

client_modbus: $(COMMON) src/client/client.o src/client/clientcommand.o src/mbclient.o src/mbdev.o src/common/ringbuffer.o src/client/signalhelper.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

upload: $(PROGRAMS) signals.cfg
	for target in $^; do scp $$target root@192.168.1.121:/mnt/software/bin/; done;

clean:
	rm -f $(PROGRAMS)
	rm -f $(COMMON)
	find . -name '*.o' -exec rm \{\} \;
