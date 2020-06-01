# simple-chat

These programs are most effective when ran in separate tabs/windows

First navigate to the folder where you have saved these files 

Then start the server by typing:

python3 chatserve.py PORTNUMBER

EXAMPLE: python3 chatserve.py 50000

Next compile chatclient.c by typing "make" on the command line

then run the client by typing:

./chatclient SERVERHOSTNAME PORTNUMBER   <--- SAME PORT AS SERVER

NOTE: the server host argument needs to be the name of the host that the server is running one

The client sends the first message and then server is prompted to respond
Subsequent messages cannot be sent until a response is received
Messages cannot be empty or greater than 500 characters

To end the chat from either side send the message "/quit"

The server can be quit via a SIGINT (^c)

I hope you enjoy this simple chat program!
