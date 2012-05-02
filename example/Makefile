CC=gcc
CFLAGS=-g

echosrv:echosrv.exe
	./echosrv.exe	

echosrv.exe:echosrv.o websocket.o handshake.o acrypto.o util.o dataframe.o
	$(CC) $(CFLAGS) -o echosrv.exe echosrv.o websocket.o handshake.o acrypto.o util.o dataframe.o -lssl -lpthread

echosrv.o:echosrv.c ../src/websocket.h ../src/handshake.h ../src/util.h
	$(CC) $(CFLAGS) -c echosrv.c -I../src

websocket.o:../src/websocket.c ../src/websocket.h
	$(CC) $(CFLAGS) -c ../src/websocket.c -I../src

handshake.o:../src/handshake.c ../src/handshake.h ../src/acrypto.h ../src/util.h
	$(CC) $(CFLAGS) -c ../src/handshake.c -I../src

dataframe.o:../src/dataframe.c ../src/dataframe.h
	$(CC) $(CFLAGS) -c ../src/dataframe.c -I../src

util.o:../src/util.c ../src/util.h
	$(CC) $(CFLAGS) -c ../src/util.c -I../src

acrypto.o:../src/acrypto.c ../src/acrypto.h
	$(CC) $(CFLAGS) -c ../src/acrypto.c -I../src
