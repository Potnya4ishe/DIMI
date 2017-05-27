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

/**
 * @brief function-reaction on a received signal
 * 
 * @param signo is number of received signal
 * 
 * @return 0 on success
 */
void main_signal(int signo);


/**
 * @brief calculates the integral of the given fucntion via distributed calculations.
 *		Looks for servers in the local network, divides the problem into equal 
 *		parts and sends this data to the servers, which calculate their
 * 		parts and send back the results.
 * 
 * @param function is the function to calculate the integral of
 *
 * @param a is the left end of the interval
 *
 * @param b is the right end of the interval
 * 
 * @return the value of the integral
 */
double client_integrate(char* function, double a, double b);


/**
 * @brief prepares the UDP-socket connection to be established. Via this socket
 *		the broadcast will be send to find all available servers.
 *
 * 
 * @return socket-id of prepared UDP-socket
 */
int prepare_UDP_socket();


/**
 * @brief prepares the message to be broadcasted and sends this message to the local network.
 *
 * @param broad_sock socket fd for the prepared UDP-socket
 * 
 * @return 0 on success
 */
int broad_cast(int broad_sock);

int collect_answers(serv_st servers[MAX_NUMB_SERV], int broad_sock);

double split_problem_and_solve(serv_st servers[MAX_NUMB_SERV], int current, double a, double b, char* function);

void* work_with_server(void* arg_box);

void prepare_signals();
