#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h> 
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>

void* work_with_server(void* arg_box);

typedef struct 
{
	struct sockaddr_in addres;
	int numb_procs;
} serv_st;



typedef struct
{
	double a;
	double b;
	long partition;
	struct sockaddr_in server;
	double* result;
	pid_t pid;
	char function[64];
} serv_thread_data;


void main_signal(int signo);

double client_integrate(char* function, double a, double b);

int prepare_UDP_socket();

int broad_cast(int broad_sock);

int collect_answers(serv_st servers[MAX_NUMB_SERV], int broad_sock);

double split_problem_and_solve(serv_st servers[MAX_NUMB_SERV], int current, double a, double b, char* function);

void* work_with_server(void* arg_box);

void prepare_signals();
