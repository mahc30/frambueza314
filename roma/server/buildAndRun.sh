!#bin/bash
rm server
gcc server.c client_handler.c server_commands.c ../util/log.c ../util/timer.c ../hw/led.c -o server -lpthread -lwiringPi
chmod +x server
./server
