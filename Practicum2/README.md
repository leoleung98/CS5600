Author: Ethan Liu and Leo Liang
Instructor: Martin Schedlbauer
Course: CS600
Spring 2023

PRACTICUM II
This is a simple file transfer application that allows users to send and receive files between a client and server using TCP sockets. The application is written in C and consists of two main components: a client and a server.

Dependencies
The application requires the following dependencies:

C compiler
POSIX-compliant operating system (e.g., Linux, macOS)
OpenSSL library
Building the Application
To build the application, follow these steps:

This program implements a server-based file storage system that allow any program to open a TCP connection through a socket, request file services from server. There are five fget commands: GET, PUT, MD, INFO, & RM
Each command is a requested from client's end to send after it connects to the server's end.

Example:
% fget GET folder/foo.txt data/localfoo.txt -> client send requeste server and to downloand data information from server

% fget PUT local/afile.c folder/gfile.c -> client send request to server and to upload data information to server

% fget MD folder/newfolder -> client send requests to create a new directory on server

% fget INFO folder/foo.txt -> client send requests to get information of a file or directory

% fget RM folder/somefile.txt -> client send requests to remove a file or directory from server


The contained files for this program:
Filepath.h -> Header file for filepath.c and it can be used as an interface for other programs
Filepath.c -> Functions to mirror data informations from primary usb drive to backup usb drive
server.c -> Server program to received message through TCP protocol form client program and uses filepath program to manage all the data information stores in primary and back usb drive
client.c -> Client program to send and to receive message or data information from server.

To run the server, follow these steps:

1. Navigate to the config.txt and to make sure primary usb drive and backup usb drive paths are written in the corrected order. 
2. Run the MAKE file to create c programming executables
3. Run the command ./server <port> to start the server, where <port> is the port number to listen on.
5. Run the command ./client <server-address> <server-port> <filename> to send a file to the server, where <server-address> is the IP address or hostname of the server, <server-port> is the port number on which the server is listening, and <filename> is the name of the file to be transferred.
4. Input and command in format of "fget COMMAND server/path client/path