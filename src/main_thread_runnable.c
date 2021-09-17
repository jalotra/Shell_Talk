#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "list.h"
#include "network.h"
#include "io.h"


#define MAX_SIZE 513

// Global sendList and receiveList lists
List *sendList, *receiveList;
void malloc_lists(){
    sendList = List_create();
    assert(sendList != NULL);
    receiveList = List_create();
    assert(receiveList != NULL);
}
// Global variables for hostName, hostPortNumber, remoteHostName
char* hostPortNumber = NULL;
char* remoteHostName = NULL;
char* remotePortNumber = NULL;

// Global Network DS needed
int hostSockFileDescriptor, remoteSockFileDescriptor;
struct addrinfo hostSockAddrInfo, remoteSockAddrInfo;

// For purpose of checking the packet received is from remoteHost or not
struct sockaddr_storage theirAddr;
socklen_t theirAddrLen = sizeof theirAddr;
const int MAX_BUF_LENGTH = 512 + 1;

void generate_network_details(){
    // If I am sending data :
    // 1. remoteSockFileDescriptor 2. remoteSockAddr

    // If I am receiving data
    // Remember to bind to this hostPortNumber
    // 1. hostSockFileDescriptor 2. hostSockAddrInfo

    int hostFoundAndBound = get_sock_fd_addr("localhost", hostPortNumber, &hostSockFileDescriptor, &hostSockAddrInfo, 1);
    assert(hostFoundAndBound == NETDB_SUCCESS);

    int remoteFound = get_sock_fd_addr(remoteHostName, remotePortNumber, &remoteSockFileDescriptor, &remoteSockAddrInfo, 0);
    assert(remoteFound == NETDB_SUCCESS);
}


// Global Mutexes to lock, unlock critical DS : List sendList, receiveList
pthread_mutex_t  sendMutex;
pthread_mutex_t receiveMutex;

// Global threadsState
int threadsState = 1;

void* keyboard_thread(void* arg){
    char* buff = malloc(MAX_SIZE * sizeof(char));
    while(threadsState){
        int readBytes = read_from_screen(buff, MAX_SIZE * sizeof(char));
        assert(readBytes != -1);
        if(readBytes > 0){
            // If buff is just "!"
//            int breakLoop = 1;
            for(int i = 0; i < strlen(buff); i++){
                if(buff[i] == '!'){
                    threadsState = 0;
                }
            }
            if(!threadsState){
                break;
            }
            // Put the message on the sendList List
            // First Acquire the sendList list lock
            pthread_mutex_lock(&sendMutex);
            // Add this message to end of sendList List
            // First Create a new char* so that we can add this as (void *)pItem
            char* pItem = malloc(MAX_SIZE * sizeof (char));
            strcpy(pItem, buff);
            // We can do this append conditionally based on current size of list sendList.
            // Think about that
            List_append(sendList, pItem);

            pthread_mutex_unlock(&sendMutex);
        }
    }
    pthread_exit(NULL);
}

// Pops pItem from head from List[sendList] and sends it out to remote host
void* udp_send_thread(void* arg){
    while(threadsState) {
        if(!sendList->count){
            continue;
        }
        // Lock sendList mutex
        pthread_mutex_lock(&sendMutex);

        List_first(sendList);
        void* messageToSend = List_remove(sendList);
        int isSend = send_message(&remoteSockFileDescriptor, remoteSockAddrInfo.ai_addr, (char *)messageToSend);
        assert(isSend != -1);
        // Unlock sendList mutex
        pthread_mutex_unlock(&sendMutex);
    }
    pthread_exit(NULL);
}

//  Gets message from remoteHost and appends it to receiveList
void* udp_receive_thread(void* arg){
    while(threadsState) {
        char* buff = malloc(MAX_SIZE * sizeof(char));
        int receiverMessage = receive_message(&hostSockFileDescriptor, (struct sockaddr*) &theirAddr, &theirAddrLen, buff, MAX_SIZE * sizeof(char));
        pthread_mutex_lock(&receiveMutex);
        char* pItem = malloc(MAX_SIZE * sizeof(char));
        strcpy(pItem, buff);
        List_append(receiveList, pItem);
        pthread_mutex_unlock(&receiveMutex);
    }
    pthread_exit(NULL);
}

void* screen_thread(void* arg){
    // If receiveList thread has items
    // Take them out in FIFO fashion
    while(threadsState){
        if(!receiveList->count){
            continue;
        }
        // Get the receiveList mutex
        pthread_mutex_lock(&receiveMutex);

        List_first(receiveList);
        void* messageToPrint = List_remove(receiveList);
        assert(messageToPrint != NULL);

        // Write them to screen
        int bytesWritten = write_to_screen((char *)messageToPrint);

        // Free messagetoPrint
        free(messageToPrint);

        // Unlock receiveList mutex
        pthread_mutex_unlock(&receiveMutex);
    }
    pthread_exit(NULL);
}


int main(int argc, char* argv[]){
    fprintf(stderr, "Program Started!\n");

    if(argc != 4){
        fprintf(stderr, "Usage : ./{filename} hostPortNumber remoteHostName remotePortNumber");
        exit(0);
    }
    hostPortNumber = argv[1];
    remoteHostName = argv[2];
    remotePortNumber = argv[3];
    printf("Port Number assigned!");

    malloc_lists();
    generate_network_details();


    int threadIds[] = {1, 2, 3, 4};
    pthread_t tid[4];
    pthread_attr_t threadAttr;

    pthread_create(&tid[0], NULL, &keyboard_thread, NULL);
    pthread_create(&tid[1], NULL, &udp_send_thread, NULL);
    pthread_create(&tid[2], NULL, &udp_receive_thread, NULL);
    pthread_create(&tid[3], NULL, &screen_thread, NULL);

    // Join 4 threads all of them return NULL
    for(int i = 0; i < 4; i++) {
        pthread_join(tid[i], NULL);
    }
}