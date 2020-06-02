struct message
{
    char from;
    char to;
    char message[1024];
};

struct message_buffer
{
    char message[1024];
    struct message_buffer *next;
};

struct ring_buffer
{
    char name;
    int sockfd;
    int beacon_flag;
    struct message_buffer *buf;
    struct ring_buffer *next;
};

void add_node(struct ring_buffer *node_ptr, char name, int sockfd, int flag, struct message_buffer *buf);

struct ring_buffer *find_node(struct ring_buffer *head, char name);

void trace_all(struct ring_buffer *head);

void *create_shared_memory(size_t size);