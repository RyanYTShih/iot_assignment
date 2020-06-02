all: shmem.o ring_buffer.o
	gcc -o shmem shmem.o ring_buffer.o

shmem.o:
	gcc -c shmem.c

ring_buffer.o:
	gcc -c ring_buffer.c

clean:
	rm shmem.o ring_buffer.o