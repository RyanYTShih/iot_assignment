all: apx.o client.o ring_buffer.o
	gcc -o apx apx.o ring_buffer.o -lpthread
	gcc -o client client.o -lpthread

apx.o:
	gcc -c apx.c

client.o:
	gcc -c client.c

ring_buffer.o:
	gcc -c ring_buffer.c

clean:
	rm apx.o client.o ring_buffer.o
