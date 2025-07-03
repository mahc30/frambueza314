!#bin/bash
rm sr
gcc serverRewrite.c client_handler.c ../util/log.c -o sr -lpthread
chmod +x sr
./sr
