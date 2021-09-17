//
// Created by shivam on 6/21/21.
//

#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>

int get_sock_fd_addr(char* hostName, char* hostPortNumber, void* sockFileDescriptor, void* sockAddrInfo, int wantToBind);

int send_message(void* sockFileDescriptor, void* sockAddr, char* message);

int bind_socket(void* sockFileDescriptor, void* sockAddrInfo);

int receive_message(void* sockFileDescription, void* sockAddr, void* sockAddrLen, char* buf, int maxBytes);

int close_socket(void* sockFileDescriptor);

void* get_in_addr(struct sockaddr* sockAddr);


#endif 
