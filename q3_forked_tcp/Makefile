all:
	gcc server.c -o build/server -lm -Wall
	gcc client.c -o build/client -lm -Wall

	chmod +x build/server
	chmod +x build/client


clean:
	-rm build/server
	-rm build/client