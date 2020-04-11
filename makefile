all: chatclient.c
	gcc -o chatclient chatclient.c
clean:
	$(RM) chatclient
