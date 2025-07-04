!#bin/bash
rm sr
gcc server.c client_handler.c ../util/log.c -o server -lpthread
chmod +x sr
./server
