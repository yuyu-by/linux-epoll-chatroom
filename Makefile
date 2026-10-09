all: server client

server: server.c net_utils.c
	gcc -Wall -Wextra server.c net_utils.c -o server

client: client.c net_utils.c
	gcc -Wall -Wextra client.c net_utils.c -o client
