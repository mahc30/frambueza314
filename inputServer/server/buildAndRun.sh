!#bin/bash
rm server
gcc server.c client_handler.c ../util/log.c -o server -lpthread
chmod +x server
./server
