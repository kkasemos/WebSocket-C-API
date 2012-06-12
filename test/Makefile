CC=gcc
CFLAGS=-g

test_all:test_handshake test_util test_acrypto test_dataframe

test_dataframe:test_dataframe.exe
	./test_dataframe.exe

test_acrypto:test_acrypto.exe
	./test_acrypto.exe

test_util:test_util.exe
	./test_util.exe

test_handshake:test_handshake.exe
	./test_handshake.exe

test_websocket:test_websocket.exe
	./test_websocket.exe

test_websocket.exe:test_websocket.o websocket.o handshake.o acrypto.o util.o dataframe.o
	$(CC) $(CFLAGS) -o test_websocket.exe test_websocket.o websocket.o handshake.o acrypto.o util.o dataframe.o /usr/local/lib/libcheck.a -lssl -lpthread

test_websocket.o:test_websocket.c ../src/websocket.h
	$(CC) $(CFLAGS) -c test_websocket.c -I../src

websocket.o:../src/websocket.c ../src/websocket.h
	$(CC) $(CFLAGS) -c ../src/websocket.c -I../src

test_handshake.exe:test_handshake.o handshake.o acrypto.o util.o
	$(CC) $(CFLAGS) -o test_handshake.exe test_handshake.o handshake.o acrypto.o util.o /usr/local/lib/libcheck.a -lssl

test_handshake.o:test_handshake.c ../src/handshake.h ../src/acrypto.h
	$(CC) $(CFLAGS) -c test_handshake.c -I../src

handshake.o:../src/handshake.c ../src/handshake.h ../src/acrypto.h ../src/util.h
	$(CC) $(CFLAGS) -c ../src/handshake.c -I../src

test_util.exe:test_util.o util.o
	$(CC) $(CFLAGS) -o test_util.exe test_util.o util.o /usr/local/lib/libcheck.a

test_util.o:test_util.c ../src/util.h
	$(CC) $(CFLAGS) -c test_util.c -I../src

util.o:../src/util.c ../src/util.h
	$(CC) $(CFLAGS) -c ../src/util.c -I../src

test_acrypto.exe:test_acrypto.o acrypto.o
	$(CC) $(CFLAGS) -o test_acrypto.exe test_acrypto.o acrypto.o /usr/local/lib/libcheck.a -lssl

test_acrypto.o:test_acrypto.c ../src/acrypto.h
	$(CC) $(CFLAGS) -c test_acrypto.c -I../src

acrypto.o:../src/acrypto.c ../src/acrypto.h
	$(CC) $(CFLAGS) -c ../src/acrypto.c -I../src

test_dataframe.exe:test_dataframe.o dataframe.o util.o
	$(CC) $(CFLAGS) -o test_dataframe.exe test_dataframe.o dataframe.o util.o /usr/local/lib/libcheck.a

test_dataframe.o:test_dataframe.c ../src/dataframe.h ../src/util.h
	$(CC) $(CFLAGS) -c test_dataframe.c -I../src

dataframe.o:../src/dataframe.c ../src/dataframe.h
	$(CC) $(CFLAGS) -c ../src/dataframe.c -I../src

clean:
	rm *.o
	rm *.exe