#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "ring_buffer.h"

void *create_shared_memory(size_t size)
{
    // Our memory buffer will be readable and writable:
    int protection = PROT_READ | PROT_WRITE;

    // The buffer will be shared (meaning other processes can access it), but
    // anonymous (meaning third-party processes cannot obtain an address for it),
    // so only this process and its children will be able to use it:
    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    // The remaining parameters to `mmap()` are not important for this use case,
    // but the manpage for `mmap` explains their purpose.
    return mmap(NULL, size, protection, visibility, -1, 0);
}

void add_node(struct ring_buffer *node_ptr, char name, int sockfd, int flag, struct message_buffer *buf)
{
    void *shmem = create_shared_memory(sizeof(struct ring_buffer));

    struct ring_buffer *node = malloc(sizeof(struct ring_buffer));
    node->name = name;
    node->sockfd = sockfd;
    node->beacon_flag = flag;
    node->buf = buf;
    node->next = NULL;

    memcpy(shmem, node, sizeof(node));

    while (node_ptr->next != NULL)
    {
        node_ptr = node_ptr->next;
    }

    node_ptr->next = shmem;
    // node_ptr->next = node;
}

struct ring_buffer *find_node(struct ring_buffer *head, char name)
{
    struct ring_buffer *ptr = head;
    while (ptr->name != name)
    {
        ptr = ptr->next;
        if (ptr == NULL)
            return NULL;
    }
    return ptr;
}

void trace_all(struct ring_buffer *head)
{
    // 5 clients (temp)
    int n = 5;
    // while (head != NULL)
    while (n--)
    {
        printf("name=%c, beacon_flag=%d, ", head->name, head->beacon_flag);
        if (head->sockfd != 0)
            printf("sockfd=%d, ", head->sockfd);
        printf("\n");
        head = head->next;
    }
}