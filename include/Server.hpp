#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h> 
#include <netinet/ip.h>
#include <string>

typedef struct sembuf semoper;

struct st{
	double a;
	double b;
	long number;
	long procs;
	double result;
	long partition;
	ASTree* tree;
};
typedef struct st info_struct;

typedef struct
{
	int client;
	pthread_t* threads;
	int procs;
} alive_data;


typedef struct
{
	int semid;
	int procs;
} broad_data;


void* calculate( void* arg_box);

void* broad_thread_func(void* arg);

void* is_alive(void* arg);

void thread_signal(int signo)
{
	pthread_exit(PTHREAD_CANCELED);
}

double integrate( double (*func)(double, ASTree*), double a, double b, int procs, int partition, char* function, int client, int* status);

double func (double x, ASTree* tree);
