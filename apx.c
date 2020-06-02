#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "ring_buffer.h"

#define MAX_CLIENT_NUM 5

void *shmem;
struct ring_buffer *ring;
int count = 0;

char client_list[MAX_CLIENT_NUM] = {
    'A', 'B', 'C', 'D', 'E'
};

// void *create_shared_memory(size_t size)
// {
//     // Our memory buffer will be readable and writable:
//     int protection = PROT_READ | PROT_WRITE;

//     // The buffer will be shared (meaning other processes can access it), but
//     // anonymous (meaning third-party processes cannot obtain an address for it),
//     // so only this process and its children will be able to use it:
//     int visibility = MAP_SHARED | MAP_ANONYMOUS;

//     // The remaining parameters to `mmap()` are not important for this use case,
//     // but the manpage for `mmap` explains their purpose.
//     return mmap(NULL, size, protection, visibility, -1, 0);
// }

void init_ring(struct ring_buffer *head)
{
    int i;
    for (i = 1; i < MAX_CLIENT_NUM; ++i)
    {
        add_node(head, client_list[i], NULL, 0, NULL);
    }

    struct ring_buffer *ptr = find_node(head, 'E');
    ptr->next = head;
}

void *pooling()
{
    // int n = 10;
    // while(n--) {
    //     printf("pooling?\n");
    //     sleep(1);
    // }

    char poolMsg = "sleep?";

    struct ring_buffer *ptr = shmem;
    while (1)
    {
        if (ptr->sockfd > 0)
        {
            write(ptr->sockfd, poolMsg, sizeof(poolMsg));
            // read(sockfd, )
        }
        ptr = ptr->next;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    shmem = create_shared_memory(sizeof(struct ring_buffer));

    ring = malloc(sizeof(struct ring_buffer));
    ring->name = client_list[0];
    ring->sockfd = NULL;
    ring->beacon_flag = 1;
    ring->buf = NULL;
    ring->next = NULL;
    memcpy(shmem, ring, sizeof(ring));
    init_ring((struct ring_buffer *)shmem);
    // init_ring(ring);

    // // test for shared memory
    // pid_t p = fork();
    // switch(p) {
    //     case -1:
    //         printf("fork error.\n");
    //         exit(-1);
    //         break;

    //     case 0:
    //         sleep(2);
    //         printf("this is child, pid=%d\n", getpid());
    //         trace_all(shmem);
    //         printf("\n");
    //         ((struct ring_buffer *)shmem)->next->name = 'W';
    //         // ring->next->name = 'W';
    //         exit(0);
    //         break;

    //     default:
    //         ((struct ring_buffer *)shmem)->next->name = 'F';
    //         // ring->next->name = 'F';
    //         wait(NULL);
    //         trace_all(shmem);
    //         printf("\n");
    //         break;
    // }
    // exit(0);

    // pthread_t poolingThread;
    // pthread_create(&poolingThread, NULL, pooling, NULL);
    // trace_all(shmem);

    // pthread_join(poolingThread, NULL);
    // printf("join\n");
    // sleep(10);
    // exit(0);
    //socket的建立
    char inputBuffer[1024] = {};
    char message[] = {"Hi,this is server."};
    int sockfd = 0, forClientSockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("Fail to create a socket.");
    }

    //socket的連線
    struct sockaddr_in serverInfo, clientInfo;
    int addrlen = sizeof(clientInfo);
    bzero(&serverInfo, sizeof(serverInfo));

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(8700);
    bind(sockfd, (struct sockaddr *)&serverInfo, sizeof(serverInfo));
    listen(sockfd, 5);

    while (1)
    {
        forClientSockfd = accept(sockfd, (struct sockaddr *)&clientInfo, &addrlen);
        struct ring_buffer *ptr_client = find_node(shmem, client_list[count++]);
        ptr_client->sockfd = forClientSockfd;

        pid_t pid = fork();
        switch (pid)
        {
        case -1:
            printf("fork error.\n");
            exit(-1);
            break;

        case 0:
            printf("this is child, pid=%d\n", getpid());
            trace_all(shmem);
            do
            {
                write(forClientSockfd, message, sizeof(message));
                read(forClientSockfd, inputBuffer, sizeof(inputBuffer));
                char to_client = inputBuffer[0];
                struct ring_buffer *ptr_to = find_node(shmem, to_client);
                if (ptr_to != NULL && ptr_to->sockfd > 0)
                {
                    printf("Message from %c to %c: %s\n", ptr_client->name, to_client, inputBuffer);
                    write(ptr_to->sockfd, inputBuffer, sizeof(inputBuffer));
                }

            } while (strcmp(inputBuffer, "ex") != 0);
            ptr_client->sockfd = NULL;
            printf("client %c left.\n", ptr_client->name);
            exit(0);
            break;

        default:
            break;
        }

        // write(forClientSockfd,message,sizeof(message));
        // read(forClientSockfd,inputBuffer,sizeof(inputBuffer));
        // printf("Get:%s\n",inputBuffer);
        // close(forClientSockfd);
    }
    return 0;
}