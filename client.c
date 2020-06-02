#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

int sockfd = 0;

char beacon_flag = 0;

void *readMessage()
{
    // int sockfd = *(int*)data;
    char receiveMsg[1024];
    // char bb[] = "1";
    while (1)
    {
        read(sockfd, receiveMsg, sizeof(receiveMsg));
        if (strcmp(receiveMsg, "sleep?") == 0)
        {
            if (beacon_flag == 0)
            {
                char bb[] = "0";
                write(sockfd, bb, sizeof(bb));
            }
            else
            {
                char bb[] = "1";
                write(sockfd, bb, sizeof(bb));
            }
        }
        else
        {
            printf("%s\n", receiveMsg);
            // fflush(NULL);
        }
    }
}

void *writeMessage()
{
    char message[1024];
    // do
    // {
    //     // printf("send: ");
    //     scanf("%s", message);
    //     if (strcmp(message, "wake") == 0)
    //     {
    //         beacon_flag = 0;
    //         printf("beacon_flag set 0: wake\n");
    //     }
    //     else if (strcmp(message, "sleep") == 0)
    //     {
    //         beacon_flag = 1;
    //         printf("beacon_flag set 1: sleep\n");
    //     }
    //     else
    //     {
    //         write(sockfd, message, sizeof(message));
    //     }

    //     // read(sockfd, receiveMessage, sizeof(receiveMessage));

    //     // printf("%s", receiveMessage);
    // } while(strcmp(message, "ex") != 0);
}

int main(int argc, char *argv[])
{

    //socket的建立
    // int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("Fail to create a socket.");
    }

    //socket的連線

    struct sockaddr_in info;
    bzero(&info, sizeof(info));
    info.sin_family = AF_INET;

    //localhost test
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(8700);

    int err = connect(sockfd, (struct sockaddr *)&info, sizeof(info));
    if (err == -1)
    {
        printf("Connection error");
    }
    // printf("sock: %d\n", sockfd);
    pthread_t readThread, writeThread;
    pthread_create(&readThread, NULL, readMessage, NULL);
    // pthread_create(&writeThread, NULL, writeMessage, NULL);

    //Send a message to server
    char message[1024];
    char receiveMessage[1024] = {};
    // while(1) {
    //     scanf("%s", message);

    // }
    while (1)
    {
        // printf("send: ");
        scanf("%s", message);
        if (strcmp(message, "wake") == 0)
        {
            beacon_flag = 0;
            printf("beacon_flag set 0: wake\n");
        }
        else if (strcmp(message, "sleep") == 0)
        {
            beacon_flag = 1;
            printf("beacon_flag set 1: sleep\n");
        }
        else
        {
            write(sockfd, message, sizeof(message));
        }

        // read(sockfd, receiveMessage, sizeof(receiveMessage));

        // printf("%s", receiveMessage);
    }
    pthread_join(readThread, NULL);
    // pthread_join(writeThread, NULL);
    printf("close Socket\n");
    close(sockfd);
    return 0;
}
