socket_api: socket_api.c
	gcc -c -o socket_api.o  socket_api.c
	ar rcs libsocket_api.a socket_api.o
	cp libsocket_api.a /usr/lib
	cp socket_api.h /usr/include