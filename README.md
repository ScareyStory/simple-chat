# simple-chat

These programs assume that the user has access to OSU's flip3 network
If you do not have access, change the HOST name in the code from flip3 to a host
of your choice. 'localhost' will allow you to communicate on your own machine.

These programs are most effective when ran in separate tabs/windows
To run in the same window the background process key '&' must be used

First navigate to the folder where you have saved these files 

Then start the server by typing:

python3 chatserve.py <port>


Next compile chatclient.c by typing "make" on the command line

then run the client by typing:

./chatclient flip3 <port>   <--- SAME PORT AS SERVER

NOTE: Use of the argument flip3 is referenced at the top of this file


The client sends the first message and then server is prompted to respond
Subsequent messages cannot be sent until a response is received
Messages cannot be empty or greater than 500 characters

To end the chat from either side send the message "/quit"

The server can be quit via a SIGINT (^c)

I hope you enjoy this simple chat program!
