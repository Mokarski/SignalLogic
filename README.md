# SignalLogic
Cloning into # SignalLogic...
# SignalLogic

Concept of this solution is subscribe of sifnals for client/server operation.
Build project by command:
make // for make project
make upload // for upload to arm

Structure of project:
common // library for server and client using
client // client side
server //server side

*client.c // template for client creation

Signals:
specification of signals
dev.NAME //dev. mandatory prefix NMAE - the name of signal. Length of NAME not limited.

dev.NAME, init_value, 0/1, bit/int, Modbus_id, Mb_reg, Bit_pos, Marker
                      0-read 1-write                   |	|
                    					Position of bit if signal type is bit
                    						|
                    						 Marker - 1 symbol for routing