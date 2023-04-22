all: server client

server: server.c filepath.c
	gcc server.c filepath.c -o server -lpthread

client: client.c
	gcc client.c -o client

clean:
	rm -f server client
