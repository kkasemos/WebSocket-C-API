test_websocket:test_websocket.o websocket.o
	gcc -g -o test_websocket test_websocket.o websocket.o /usr/local/lib/libcheck.a -lpthread
test_websocket.o:test_websocket.c ../src/websocket.h
	gcc -g -c test_websocket.c -I../src
websocket.o:../src/websocket.c ../src/websocket.h
	gcc -g -c ../src/websocket.c -I../src