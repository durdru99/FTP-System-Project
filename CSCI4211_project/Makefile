#	gcc -g -o client -fsanitize=address CSCI4211PJ1_client.c
# this is the make up code to compile the client using GCC -0 
client: CSCI4211PJ1_client.c
	gcc -o client CSCI4211PJ1_client.c
	./client

# this is the make up code to compile the sever
server: CSCI4211PJ1_serve.c
	gcc -o server CSCI4211PJ1_serve.c
	./server

# remove all compiled files ( client & server)
clean:
	rm   client server
	rm -f   client server