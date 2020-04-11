################################################################################
## Author:        Story Caplain
## Filename:      chatserve.py
## Description:   This program hosts a connection on a specified port number.
##                It utilizes the socket API to communicate to a peer via TCP.
## Course name:   CS 372
## Last Modified: 02/08/2020
################################################################################

import sys
import socket
import time
import signal

# Sources:
# https://realpython.com/python-sockets/
# https://www.geeksforgeeks.org/socket-programming-python/
# https://stackoverflow.com/questions/2444459/python-sock-listen
# https://stackoverflow.com/questions/12454675/whats-the-return-value-of-socket-accept-in-python
# https://docs.python.org/3/library/internet.html
# https://stackoverflow.com/questions/1112343/how-do-i-capture-sigint-in-python

################################################################################
## Function name: signal_handler
## parameters:    This function takes custom signal handlers
## returns:       Nothing, this function servers to catch the sigint to avoid
##                an ugly error message upon SIGINTing this py file
################################################################################
def signal_handler(signal, frame):
    print("\n\nYou have shut down the server, goodbye...\n")
    sys.exit(0)

################################################################################
## Function name: receive
## parameters:    Takes in the socket to be communicated with as an argument
##                This socket was returned by an accept call in main
## returns:       This function returns either -1, 1, or 0
##                -1 means the communication failed
##                1 means that the other host chose to quit the chat
##                0 means that a valid message was received
################################################################################
def receive(c):

    # call recv on the connected to socket
    # allowing for up to a 500 character message
    # would otherwise have to implement message length handshakes
    # and loop recv until all message bytes are received
    # here it is safe to assume that messages will not be very large
    message = c.recv(550)
    message = message.decode()
    message = message.replace("\n","")

    # if the recv call failed return -1
    if(len(message) <= 0):
        print("\nInitial recv call failed, trying one more time...")
        message = c.recv(550)

        # if still fails, return -1
        if(len(message) <= 0):
            print("Second failure, terminating chat")
            return(-1) 

    # if the message received is "\quit", return 1
    if("/quit" in message):
        print("\nClient quit the chat",end="")
        return 1

    # else print the message sent
    else:
        print("\n"+message)

    return 0

################################################################################
## Function name: sending
## parameters:    Takes as params the connection socket and the user's handle
##                This socket was returned by a call to accept in main
## returns:       This function returns either a 1 or 0
##                1 means that the message "\quit" was sent over
##                0 means that a valid message was sent over
################################################################################
def sending(c, handle):

    # get message from user and verify it is not too long or empty
    print("\n"+handle, end="")
    message = input(": ")
    while(len(message) > 500 or len(message) <= 0):
        print("\nMessages must be 1-500 characters long!")
        print(handle, end="")
        message = input(": ")

    # send over the message
    message = handle + ": " + message
    c.send(message.encode('utf-8'))

    # if user entered \quit
    if("/quit" in message):
        print("You have quit the chat",end="")
        return 1

    return 0

################################################################################
###################### END OF FUNCTIONS START OF MAIN ##########################
################################################################################


# start by making sure that the correct number of args has been passed in
if len(sys.argv) != 2:
    print("Incorrect args!")
    print("Usage: python chatserver.py <port number>")
    exit(1)

# set up signal handler
signal.signal(signal.SIGINT, signal_handler)

# introduce program and get a valid handle from user
print("\nYou have started chatserve on port", sys.argv[1])
handle = input("\nPlease enter a handle (10 character max): ")
while(len(handle) > 10):
    print("Handle too long!")
    handle = input("Please enter a handle (10 character max): ")

# store port number as int
PORT = int(sys.argv[1])

# store host, blank allows any address to connect
HOST = ''

# setup up a socket using ipv4 and TCP
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# bind the host and port number to the socket
s.bind((HOST, PORT))

# listen to the socket, allow up to 5 queued connections
s.listen(5)

# Tell user we have finished setting up Host A
print("\nServer has finished setup! Ready to establish communication",end="")

while(1):

    # call accept on the server socket 's'
    # returns the other host's socket and store it in 'c'
    # also returns the port of host at 'c' and stores it in addr
    print("\n\nAwaiting message from new client...",end="")
    c, addr = s.accept()

    # Begin chat loop, chat loops until user quits or connection dies
    while(1):
    
        # call receive function on c to receive message
        # store the result of the call in message    
        message = receive(c)
    
        # a return value of -1 means the communication failed, break
        if(message == -1):
            print("\nConnection failure...")
            c.close()
            time.sleep(1)
            break
    
        # a return value of 1 means the peer chose to quit the chat
        if(message == 1):
            c.close()
            time.sleep(1)
            break
    
        # give user option to send a message back by calling sending function
        retval = sending(c, handle)
    
        # a return value of -1 means the communication failed, break
        if(retval == -1):
            print("\nConnection failure on send...")
            c.close()
            time.sleep(1)
            break
    
        # check if user entered quit in sending function
        if(retval == 1):
            c.close()
            time.sleep(1)
            break

