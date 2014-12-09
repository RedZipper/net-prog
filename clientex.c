#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define MSG_SIZE 1024

int main(int argc, char *argv[])    {
    // declare.
    int newFD, bytesrecvd, fdmax;
    char msg[MSG_SIZE];
    fd_set master_set, read_set;
    struct addrinfo hints, *srvrAI;

    // clear sets
    FD_ZERO(&master_set);
    FD_ZERO(&read_set);

    // clear hints
    memset(&hints, 0, sizeof hints);

    // prep hints
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // get srver info
    if(getaddrinfo(argv[1], argv[2], &hints, &srvrAI) != 0) {
        perror("* ERROR | getaddrinfo()\n");
        exit(1);
    }

    // get a socket
    if((newFD = socket(srvrAI->ai_family, srvrAI->ai_socktype, srvrAI->ai_protocol)) == -1) {
        perror("* ERROR | socket()\n");
        exit(1);    
    }

    // connect to server
    if(connect(newFD, srvrAI->ai_addr, srvrAI->ai_addrlen) == -1)   {
        perror("* ERROR | connect()\n");
        exit(1);    
    }

    // add to master, and add keyboard
    FD_SET(newFD, &master_set);
    FD_SET(STDIN_FILENO, &master_set);

    // initialize fdmax
    if(newFD > STDIN_FILENO)
        fdmax = newFD;
    else
        fdmax = STDIN_FILENO;

    while(1)    {
        // equate
        read_set = master_set;

        if(select(fdmax+1, &read_set, NULL, NULL, NULL) == -1)  {
            perror("* ERROR | select()");
            exit(1);
        }

        // check server
        if(FD_ISSET(newFD, &read_set))  {
            // read data
            if((bytesrecvd = recv(newFD, msg, MSG_SIZE, 0)) < 0 )   {
                perror("* ERROR | recv()");
                exit(1);
            }
            msg[bytesrecvd] = '\0';

            // print
            fprintf(stdout, "%s\n", msg);
        }

        // check keyboard
        if(FD_ISSET(STDIN_FILENO, &read_set))   {
            // read data from stdin
            if((bytesrecvd = read(STDIN_FILENO, msg, MSG_SIZE)) < 0)    {
                perror("* ERROR | read()");
                exit(1);
            }
            msg[bytesrecvd] = '\0';


            // send
            if((send(newFD, msg, bytesrecvd, 0)) == -1) {
                perror("* ERROR | send()");
                exit(1);
            }
        }
    }

    return 0;
}
