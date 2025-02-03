#	gcc -g -o client -fsanitize=address CSCI4211PJ1_client.c
client: CSCI4211PJ1_client.c
	gcc -o client CSCI4211PJ1_client.c
	./client

server: CSCI4211PJ1_serve.c
	gcc -o server CSCI4211PJ1_serve.c
	./server

clean:
	rm   client server
	rm -f   client server