/*
 Functionality: 
 * Note : I will only add support for IPV4 addresses and UDP protocol.
 * Add network related stuff
 - Example : Search for a IPV4 address, connect to a socket(bind).
 - Send and recieve message to and from a sockaddr File Descriptor.
*/
#include "network.h"

#define FAILURE -1
#define SUCCESS 0
#define handle_error(msg) \
           do { perror(msg); exit(FAILURE); } while (0)


// Returns a 0 if we can get some sockaddr based on hostName, hostPortNumber, wantToBind
int
get_sock_fd_addr(char* hostName, char* hostPortNumber, void* sockFileDescriptor, void* sockAddrInfo, int wantToBind){
    int sockfd;
    struct addrinfo hints, *res, *p;

    // Fill hints with things that I want to get
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;

    int status;
    if((status = getaddrinfo(hostName, hostPortNumber, &hints, &res)) != 0){
        handle_error(gai_strerror(status));
    }

    // loop through the res and take the first one that I can make
    for(p = res; p != NULL; p = p->ai_next){
        if( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ){
            continue;
        }else{
            if(wantToBind == 1){
                if(bind_socket(&sockfd, p) == -1){
                    continue;
                }
            }
            break;
        }
    }

    if(p == NULL){
        handle_error("Can't estlabish a socket connection.");
        fprintf(stderr, "Can't establish a socket connection with %s on port %s\n", hostPortNumber, hostName);
    }
    // Else return sockfd and p
    *(int *)sockFileDescriptor = sockfd;
    *(struct addrinfo *)sockAddrInfo = *p;

    assert(sockFileDescriptor != NULL);
    assert(sockAddrInfo != NULL);

    return SUCCESS;
}

// Returns the number of bytes send to destination sockAddrInfo
int
send_message(void* sockFileDescriptor, void* sockAddr, char* message){
    int bytesToSend = (int)strlen(message);
//    socklen_t sockAddrLen = ;
    int bytesActuallySend = sendto(*(int *)sockFileDescriptor,
                                  message,
                                  bytesToSend,
                                  0,
                                  sockAddr,
                                   sizeof(*((struct sockaddr *)sockAddr)));

    assert(bytesActuallySend != -1);
    return bytesActuallySend;
}

// Bind to socket
int bind_socket(void* sockFileDescriptor, void* sockAddrInfo){
    if(bind(*(int *)sockFileDescriptor, ((struct addrinfo *)sockAddrInfo)->ai_addr, ((struct addrinfo *)sockAddrInfo)->ai_addrlen) == -1){
        handle_error("Binding Failed with %s ((struct addrinfo *)sockAddrInfo)->ai_addr->sa_data");
    }
    return SUCCESS;
}

// Receive bytes in char* buff from destination sockAddrInfo
int
receive_message(void* sockFileDescriptor, void* sockAddr, void* sockAddrLen, char* buf, int maxBytes){
    // Have to make sure we are bound to sockAddrInfo->ai_addr
    // No need to have addr_len
    int bytesGot = recvfrom(*(int *)sockFileDescriptor,
                            buf,
                            maxBytes - 1,
                            0,
                            ((struct sockaddr *)sockAddr),
                            (socklen_t *)sockAddr);

    // Add a NULL character so printf knows to print till here
    buf[maxBytes] = '\0';
    return bytesGot;
}

// Closes a sockFd
int close_socket(void* sockFileDescriptor){
    if(close(*(int *)sockFileDescriptor) == -1){
        handle_error("Error : Closing the socket");
    }
    return SUCCESS;
}

// Returns the address of the sockaddr sockAddr
// It is IP Version Agnostic
void* get_in_addr(struct sockaddr *sockAddr)
{
    if (sockAddr->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sockAddr)->sin_addr);
    }else{
        handle_error("Only IPV4 is supported.");
    }

}





