#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>


int forClientSockfd = 0;

void *readMessage()
{
    // int sockfd = *(int*)data;
    char receiveMsg[1024];
    while (1)
    {
        read(forClientSockfd, receiveMsg, sizeof(receiveMsg));
        printf("%s\n", receiveMsg);
        // fflush(NULL);
    }
}

void *writeMessage()
{
    char msg[1024];
    while(1) {
        // printf("send: ");
        scanf("%s", msg);
        write(forClientSockfd, msg, sizeof(msg));
        // read(forClientSockfd, recvMsg, sizeof(recvMsg));
        // printf("%s\n", recvMsg);
    }
}

int main(int argc, char *argv[])
{

    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("Fail to create a socket.");
    }

    struct sockaddr_in serverInfo, clientInfo;
    int addrlen = sizeof(clientInfo);
    bzero(&serverInfo, sizeof(serverInfo));

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(8700);
    bind(sockfd, (struct sockaddr *)&serverInfo, sizeof(serverInfo));
    listen(sockfd, 5);

    forClientSockfd = accept(sockfd, (struct sockaddr *)&clientInfo, &addrlen);
    // printf("sock: %d\n", forClientSockfd);
    pthread_t readThread, writeThread;
    pthread_create(&readThread, NULL, readMessage, NULL);
    // pthread_create(&writeThread, NULL, writeMessage, NULL);
    // pid_t pid = fork();
    // switch(pid) {
    //     case -1:
    //         printf("fork error\n");
    //         break;
    //     case 0:
            
    //         pthread_create(&readThread, NULL, readMessage, NULL);
    //         // pthread_create(&writeThread, NULL, writeMessage, NULL);
    //         char msg[1024];
    //         while(1) {
    //             // printf("send: ");
    //             scanf("%s", msg);
    //             write(forClientSockfd, msg, sizeof(msg));
    //             // read(forClientSockfd, recvMsg, sizeof(recvMsg));
    //             // printf("%s\n", recvMsg);
    //         }
    //         pthread_join(readThread, NULL);
    //         // pthread_join(writeThread, NULL);
    //         exit(0);
    //         break;
    //     default:
    //         wait(NULL);
    //         break;
    // }
    
    char msg[1024];
    char recvMsg[1024];
    // sleep(5);
    // int n = 5;
    while(1) {
        // printf("send: ");
        scanf("%s", msg);
        write(forClientSockfd, msg, sizeof(msg));
        // read(forClientSockfd, recvMsg, sizeof(recvMsg));
        // printf("%s\n", recvMsg);
    }
    
    close(sockfd);
    
}
