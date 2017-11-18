CC=gcc
CC_ARM=/opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-fsl-linux-gnueabi-gcc
COMMON=src/common/hash.c src/common/proto.c src/common/signal.c
LDFLAGS+=-lpthread -lmodbus -lrt
LDFLAGS_ARM+=-lpthread `pkg-config --libs /home/opc/Kombain/libmodbus-3.0.6/libmodbus.pc` -lrt
CFLAGS+=-g -Isrc -DMODBUS_ENABLE `pkg-config --cflags /home/opc/Kombain/libmodbus-3.0.6/libmodbus.pc`
CFLAGS_ARM+=-g -Isrc -DMODBUS_ENABLE `pkg-config --cflags /home/opc/Kombain/libmodbus-3.0.6/libmodbus.pc`

PROGRAMS_ARM=signalrouter client_logic client_modbus client_wago
PROGRAMS=client_virtual $(PROGRAMS_ARM)

LOGIC_SRC=src/logic/keyboard.c src/logic/process.c src/logic/logic_client.c src/logic/joystick.c src/logic/processor.c src/logic/local.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $^

all: $(PROGRAMS)

signalrouter: $(COMMON) src/server/signalrouter.c src/server/servercommand.c src/common/subscription.c src/server/serverevents.c
	$(CC_ARM) $(CFLAGS_ARM) -o $@ $^ $(LDFLAGS_ARM)

client_virtual: $(COMMON) src/client/client.c src/client/clientcommand.c src/virtualclient.c src/common/ringbuffer.c src/client/signalhelper.c
	#$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

client_modbus: $(COMMON) src/client/client.c src/client/clientcommand.c src/mbclient.c src/mbdev.c src/common/ringbuffer.c src/client/signalhelper.c
	#$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(CC_ARM) $(CFLAGS_ARM) -o $@ $^ $(LDFLAGS_ARM)

client_wago: $(COMMON) src/client/client.c src/client/clientcommand.c src/wagoclient.c src/mbdev.c src/common/ringbuffer.c src/client/signalhelper.c
	$(CC_ARM) $(CFLAGS_ARM) -o $@ $^ $(LDFLAGS_ARM)

client_logic: $(COMMON) src/client/client.c src/client/clientcommand.c src/logicclient.c src/common/ringbuffer.c  src/common/journal.c  src/client/signalhelper.c $(LOGIC_SRC)
	$(CC_ARM) $(CFLAGS_ARM) -o $@ $^ -lrt -lpthread
	$(CC) $(CFLAGS) -o $@-local $^ -lrt -lpthread
	#$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

upload: $(PROGRAMS_ARM) signals.cfg libmodbus.so.5.0.5 start-all 
	for target in $^; do PROGRAM=$$target $(MAKE) .upload-$$target; done;

.upload-$(PROGRAM): $(PROGRAM)
	scp $^ root@192.168.1.121:/mnt/software/bin/ && touch $@

clean:
	rm -f $(PROGRAMS)
	find . -name '*.o' -exec rm \{\} \;
