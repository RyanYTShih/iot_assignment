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

void init_ring(struct ring_buffer *head)
{
    int i;
    for (i = 1; i < MAX_CLIENT_NUM; ++i)
    {
        add_node(head, client_list[i], NULL, 0, NULL);
    }

    // circular linked list
    struct ring_buffer *ptr = find_node(head, 'E');
    ptr->next = head;
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

    // test for shared memory
    pid_t p = fork();
    switch(p) {
        case -1:
            printf("fork error.\n");
            exit(-1);
            break;

        case 0:
            sleep(2);
            printf("this is child, pid=%d\n", getpid());
            trace_all(shmem);
            printf("\n");
            ((struct ring_buffer *)shmem)->next->name = 'W';
            // ring->next->name = 'W';
            exit(0);
            break;

        default:
            ((struct ring_buffer *)shmem)->next->name = 'F';
            // ring->next->name = 'F';
            wait(NULL);
            trace_all(shmem);
            printf("\n");
            break;
    }
    
    return 0;
}