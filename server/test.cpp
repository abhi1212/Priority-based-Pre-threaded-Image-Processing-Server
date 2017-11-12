#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "csapp.h"
extern int colortogray(int, char*);
extern void echo(int connfd);


#define QUEUESIZE 2
#define LOOP 5
void *thread(void *q);


typedef struct {
	int buf[QUEUESIZE];
	long head, tail;
	int full, empty;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull, *notEmpty;
} queue;

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, int in);
void queueDel (queue *q, int *out);
void millisleep(int milliseconds);

struct sockaddr_in clientaddr;

int main(int argc, char **argv)
{

	int clientlen,clientfd,connfd,listenfd,connfd1, port;

	queue *fifo;
	pthread_t p1, p2, p3;
	struct sockaddr_in sock_var;
	fifo= queueInit();   // queueinit returns the address of structure
	if(fifo== NULL)
	{
	    printf(" Initialization Failed\n");  
	    exit(0);
	}

	if (argc != 2)
	 {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	 }

	 port = atoi(argv[1]);

	pthread_create (&p1, NULL,thread,fifo);
	pthread_create (&p2, NULL,thread,fifo);
	pthread_create (&p3, NULL,thread,fifo);
	listenfd = Open_listenfd(port);
	clientlen = sizeof(clientaddr);

	while(1)
	{
		

		connfd1 = Accept(listenfd, (SA *) &clientaddr, &clientlen);
		printf("New client received\n");
		pthread_mutex_lock (fifo->mut);
		    
		while (fifo->full)
		{
			printf ("producer: queue FULL.\n");
			pthread_cond_wait (fifo->notFull, fifo->mut);
		}
		queueAdd (fifo, connfd1);
		pthread_mutex_unlock (fifo->mut);
		pthread_cond_signal (fifo->notEmpty);


	}

}




/**************************************************************************************************************************************
Thread
***************************************************************************************************************************************/

void *thread(void *q)//thread function
{
int fd;
queue *fifo;
fifo = (queue *)q;

while(1) 
{
	pthread_mutex_lock (fifo->mut);
	while (fifo->empty) 
    	{
		printf ("consumer: queue EMPTY.\n");
		pthread_cond_wait (fifo->notEmpty, fifo->mut);
	}
	queueDel (fifo, &fd);
	pthread_mutex_unlock (fifo->mut);
	pthread_cond_signal (fifo->notFull);
	printf ("consumer: recieved %d.\n", fd);

	

millisleep(15000);
echo(fd);		
close(fd);		
}


}



queue *queueInit (void)
{
	queue *q;

	q = (queue *)malloc (sizeof (queue));
	if (q == NULL) return (NULL);

	q->empty = 1;
	q->full = 0;
	q->head = 0;
	q->tail = 0;
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (q->mut, NULL);
	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notFull, NULL);
	q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notEmpty, NULL);
	
	return (q);
}







void queueDelete (queue *q)
{
	pthread_mutex_destroy (q->mut);
	free (q->mut);	
	pthread_cond_destroy (q->notFull);
	free (q->notFull);
	pthread_cond_destroy (q->notEmpty);
	free (q->notEmpty);
	free (q);
}

void queueAdd (queue *q, int in)
{
	q->buf[q->tail] = in;
	q->tail++;
	if (q->tail == QUEUESIZE)
		q->tail = 0;
	if (q->tail == q->head)
		q->full = 1;
	q->empty = 0;

	return;
}

void queueDel (queue *q, int *out)
{
	*out = q->buf[q->head];

	q->head++;
	if (q->head == QUEUESIZE)
		q->head = 0;
	if (q->head == q->tail)
		q->empty = 1;
	q->full = 0;

	return;
}



void millisleep(int milliseconds)
{
      usleep(milliseconds * 1000);
}








 












// int clientFileDiscriptor=socket(AF_INET,SOCK_STREAM,0);
 //char str_clnt[20],str_ser[20];
 
 /*sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
 sock_var.sin_port=3000;
 sock_var.sin_family=AF_INET;
 
 if(connect(clientFileDiscriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
 {
  printf("Connected to server %dn",clientFileDiscriptor);
  printf("nEnter String to send\n");
  scanf("%s",str_clnt);
  write(clientFileDiscriptor,str_clnt,20);
  read(clientFileDiscriptor,str_ser,20);
  printf("String from Server: %s\n",str_ser);
  close(clientFileDiscriptor);
 }
 else{
  printf("Socket creation failed\n");
 }
 return 0;
}*/

