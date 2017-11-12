#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "csapp.h"
#define QUEUESIZE 2             // Defining Queue size
#define LOOP 5

void *thread(void *args);
void *producer(void *args);

int port;
queue *fifo;

typedef struct {                   // Structure of type Queue
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

//pthread_t hpt[PRIO_GROUP];
//pthread_t lpt[PRIO_GROUP];
void echo(int connfd);

int main(int argc, char **argv)
{
    struct sched_param my_param;                             
    pthread_attr_t lp_attr;
    int i, min_priority, policy;

    int listenfd, *connfd, port, clientlen;
    //queue *fifo;
    struct sockaddr_in clientaddr;
    pthread_t worker1,worker2,worker3;
   

    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    listenfd = Open_listenfd(port);
    fifo = queueInit ();
    if (fifo ==  NULL) {
        fprintf (stderr, "main: Queue Init failed.\n");
        exit (1);
    }
    my_param.sched_priority = sched_get_priority_min(SCHED_FIFO);       //returns the minimum priority value that can
                                                                        // be used with the scheduling algorithm identified by policy.
    min_priority=my_param.sched_priority;
    my_param.sched_priority++;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &my_param);         // Set scheduling policy and parameters
    pthread_getschedparam (pthread_self(), &policy, &my_param);

    pthread_attr_init(&lp_attr);
    pthread_attr_setinheritsched(&lp_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&lp_attr, SCHED_RR);
    my_param.sched_priority = min_priority;
    pthread_attr_setschedparam(&lp_attr, &my_param);
    pthread_create(&worker1,NULL,thread,fifo); 
    pthread_create(&worker2,NULL,thread,fifo);
    pthread_create(&worker3,NULL,thread,fifo);
   
     while(1){
        clientlen = sizeof(clientaddr);
        connfd=(int *)Malloc(sizeof(int));
        // clientlen = sizeof(clientaddr);
        
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        printf("new client received");
        // for (i = 0; i < 10; i++) {
        //printf("new client received");
        pthread_mutex_lock (fifo->mut);
            while (fifo->full ==1) {
                printf ("producer: queue FULL.\n");
                pthread_cond_wait (fifo->notFull, fifo->mut);
            }
            queueAdd (fifo, connfd);
            pthread_mutex_unlock (fifo->mut);
            pthread_cond_signal (fifo->notEmpty);
            printf("Server priority : %d\n",my_param.sched_priority);
        }
}


   

    void *thread (void *q)
    {
    //queue *fifo;
    int fd;

    fifo = (queue *)q;

        pthread_t thread_id = pthread_self();
        struct sched_param param;
        int priority, policy, ret;
        ret = pthread_getschedparam (thread_id, &policy, &param);
        priority = param.sched_priority;   
        while(1){
        pthread_mutex_lock (fifo->mut);
        while (fifo->empty) {
            printf ("queue EMPTY\n");
            pthread_cond_wait (fifo->notEmpty, fifo->mut);
        }
       //millisleep(3000);
        queueDel (fifo, &fd);

        pthread_mutex_unlock (fifo->mut);
        pthread_cond_signal (fifo->notFull);
        printf ("consumer: recieved %d.\n", (fd-3));
        printf("client priority : %d \n", priority);   
        millisleep(30000);
        echo(fd);//line:netp:tiny:doit
        //printf("worker served : %d \n",(fd-3));
        Close(fd);
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
    if (q->tail == q->head){
        q->full = 1;
	printf("buffer full\n");
	}
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
void *highprior_thread(void *arg){

pthread_t thread_id = pthread_self();
struct sched_param param;
int priority, policy, ret;

//pthread_barrier_wait(&mybarrier);

ret = pthread_getschedparam (thread_id, &policy, &param);
priority = param.sched_priority;   
printf("%d \n", priority);


return NULL;
}
void *lowprior_thread(void *arg){

pthread_t thread_id = pthread_self();
struct sched_param param;
int priority, policy, ret;

//pthread_barrier_wait(&mybarrier);

ret = pthread_getschedparam (thread_id, &policy, &param);
priority = param.sched_priority;   
printf("%d \n", priority);

return NULL;
}
