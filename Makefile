all: server client

server: server.c net_utils.c
	gcc -Wall -Wextra server.c net_utils.c Link.c -o server

client: client.c net_utils.c
	gcc -Wall -Wextra client.c net_utils.c Link.c -o client
