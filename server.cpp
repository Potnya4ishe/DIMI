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
//#include "common.h"
#include <string>
#include "include/Interpreter.hpp"
//#include "include/Logger.hpp"

#define PORT_RCV 38199

#define PRN_CASE(val) case val: printf("errno =" #val"\n");	break;

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

typedef struct
{
	double a;
	double b;
	long partition;
	char function[64];
} serv_data;



long partition;
void* calculate( void* arg_box);
void* broad_thread_func(void* arg);
void* is_alive(void* arg);
void thread_signal(int signo)
{
	pthread_exit(PTHREAD_CANCELED);
}
double integrate( double (*func)(double, ASTree*), double a, double b, int procs, int partition, char* function, int client, int* status);
double func (double x, ASTree* tree)
{
	//usleep(2);
	ASTnode* root = (ASTnode*)tree->getRootNode();
	return evaluate(root, x);
    //return 4/(1+x*x);//sqrt(sqrt(pow((4/(1+x*x)*(1*pow(pow(x,cos(3.1415926)),sin(x*x)-sin(x*x*(x+1)/(x+1)))*pow(sqrt(log(exp(pow(x,2)))),log(pow(exp(x+1)/exp(x),2))) +1 ))/(1+x*x),4)));																																																																																																																				
}

//int main_server(int nCores)
int main( int argc, char** argv )
{
	socklen_t addr_len = sizeof(struct sockaddr_in);
	socklen_t new_addr_len =0;
	
	int count = 1;
	
	//Подготавливаем и запускаем тред broadcast. Ловить он сразу не будет -- зависнет на семафоре. разрешим позже
	//Передаем ему количество процесоров и адрес семафоров
	//int procs = nCores;
	int procs = atoi(argv[1]);
	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	semoper can_broad = { 0, 1, 0};
	semctl (semid, 0, SETVAL, 0);
	broad_data br_data;
	br_data.semid = semid;
	br_data.procs = procs;
	pthread_t broad_thread;
	pthread_create( &broad_thread, NULL, (broad_thread_func), &br_data );
	
	//Создаем сокет для соединения с собой
	struct sockaddr_in my_addr;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = htons(PORT_RCV);
	my_addr.sin_family = AF_INET;
	int my_sock = socket( PF_INET, SOCK_STREAM, 0);
	setsockopt(my_sock, SOL_SOCKET, SO_REUSEADDR, &count, sizeof(count));
	//setsockopt(my_sock, SOL_SOCKET, SO_REUSEPORT, &count, sizeof(count));
	count = bind(my_sock, (struct sockaddr*) &my_addr, addr_len);
	if(count < 0)
		{
			printf("Fail in BIND! bind = %d\n", count);
			switch(errno){
				PRN_CASE(EBADF)
				PRN_CASE(EROFS)
				PRN_CASE(ENOTSOCK)
				PRN_CASE(ENAMETOOLONG)
				PRN_CASE(EFAULT)
				PRN_CASE(EINVAL)
				PRN_CASE(EACCES)
				PRN_CASE(ENOENT)
				PRN_CASE(ENOMEM)
				PRN_CASE(ENOTDIR)
				PRN_CASE(ELOOP)
				PRN_CASE(EADDRINUSE)
				PRN_CASE(EADDRNOTAVAIL)
			}
			exit(EXIT_FAILURE);
		}
	
	printf("socket = %d\nbind = %d\n", my_sock, count);
	
	//Разрешаем ловить broadcast
	semop(semid, &can_broad,1);
	
	//Ждем клиентa
	int client;
	struct sockaddr_in addr_client;
	serv_data data;	
	double result;
	int status;
	
	//Настраиваем приём сигналов
	struct sigaction thread_act;
	memset(&thread_act, 0, sizeof(thread_act));
	thread_act.sa_handler = thread_signal;
	sigfillset(&thread_act.sa_mask);
	sigaction(SIGUSR1, &thread_act, NULL);
	
	count = listen(my_sock, 1);
	printf("listen = %d\n", count);
	int iterations = 2;
	while(iterations > 0)
	{
		printf("Waiting for connection...\n");
		client = accept(my_sock, (struct sockaddr*) &addr_client, &new_addr_len);
		if(client <0)
		{
			printf("Fail in Accepting! accept = %d, client = %s, len = %d\n", client,inet_ntoa(addr_client.sin_addr), new_addr_len	);
			switch(errno){
				PRN_CASE(EAGAIN)
				PRN_CASE(EBADF)
				PRN_CASE(ECONNREFUSED)
				PRN_CASE(ENOTSOCK)
				PRN_CASE(EINTR)
				PRN_CASE(EFAULT)
				PRN_CASE(EINVAL)
				PRN_CASE(EOPNOTSUPP)
				PRN_CASE(ECONNABORTED)
				PRN_CASE(EMFILE)
				PRN_CASE(ENFILE)
			}
		}
			
		printf("accept = %d, client = %s, len = %d\n", client,inet_ntoa(addr_client.sin_addr), new_addr_len);
		count = recv(client, &data, sizeof(data), 0);
		if(count < 0)
		{
			printf("Fail in getting data for computing! Count = %d\n", count);
			switch(errno){
				PRN_CASE(EAGAIN)
				PRN_CASE(EBADF)
				PRN_CASE(ECONNREFUSED)
				PRN_CASE(ENOTSOCK)
				PRN_CASE(EINTR)
				PRN_CASE(EFAULT)
				PRN_CASE(EINVAL)
			}
			exit(EXIT_FAILURE);
		}
		printf("Received %d\nGot a task -- a = %lf, b = %lf, parts = %ld, func = %s\n", count, data.a, data.b, data.partition, data.function );
		result = integrate(func, data.a, data.b, procs, data.partition, data.function, client, &status);
		printf("Sending a result -- %lf. Status = %d\n", result, status);
		if(status == 1)
		{
			count = send(client, &result, sizeof(double), 0);
			printf("Result sent, %d!\n", count);
		}
		else
			printf("Client Gone before a result! Stopped all threads...\n");
		close(client);
		printf("Client is gone! Starting catching new broadcasting!\n");
		semop(semid, &can_broad, 1);
		iterations--;
	}
	semctl(semid, 0, IPC_RMID);
	close(my_sock);
	return 0;	
}


double integrate( double (*func)(double, ASTree*), double a, double b, int procs, int partition, char* function, int client, int* status)
{	
	Interpreter intpr;
	ASTree* tree = intpr.run(function);

	pthread_t* tid = (pthread_t*) malloc ((procs+1) * sizeof(pthread_t));
	info_struct* arg_box = (info_struct*) malloc (procs * sizeof(info_struct));	
    int i = 0;
    double result;
    for ( i = 0; i < procs; i ++ )
	{
		arg_box[i].a = (double) a;
		arg_box[i].b = (double) b;
		arg_box[i].number = i;
		arg_box[i].procs = procs;
		arg_box[i].partition = partition;
		arg_box[i].tree = tree;
		pthread_create( &tid[i], NULL, (calculate), (void*) &arg_box[i]);
	}
	alive_data al_data;
	pthread_t alive_thread;
	al_data.client = client;
	al_data.threads = tid;
	al_data.procs = procs;
	pthread_create(&alive_thread, NULL,  (is_alive), (void*) &(al_data));
	int* state; 
	for ( i = 0; i < procs ; i ++ )
	{
		pthread_join( tid[i], (void**) &state);
		if(state != PTHREAD_CANCELED)
			result += arg_box[i].result;
	}
	if( state == PTHREAD_CANCELED)
		*status = -1;	
	else
	{
		pthread_kill( alive_thread, SIGUSR1);
		*status = 1;
	}
	free(tid);
	free(arg_box);
	delete tree;
	return result;
}

void* calculate( void* arg_box)
{
	//struct timespec start, stop;
	//clock_gettime( CLOCK_REALTIME, &start);
	info_struct* box = (info_struct*) (arg_box);
	double a = box->a;
	double b = box->b;
	ASTree* tree = box->tree;
	long partition = box->partition;
	long number = box->number;
	long procs = box->procs;
	long parts = (long) (partition/procs);
    double step = (double) (b-a)/2/partition;
    int i = 0;//,j=0;
	double sum = 0.0;
	a+= 2*step*(partition/procs)*(number);
    //printf( "Nubmer %ld, a = %lf, b= %lf, procs = %ld, parts = %ld, step = %lf\n", number, a, b, procs, parts, step);
    for ( i = 0; i < parts;  i++ )
    {
        sum+=func(a, tree);					// + 4*func(a+step) + func( a + 2*step);
        a+= step;
        sum+=4*func(a, tree);
        a+= step;
        sum+= func(a, tree);
        /*for(j = 0; j<2; j++)
			sum += func(100);*/
	}
    box->result = sum*step/3;
    //printf( "Nubmer %ld, a = %lf,  result= %lf\n", number,a, sum);
    //clock_gettime(CLOCK_REALTIME, &stop);
	//double time = (double) (stop.tv_sec - start.tv_sec) * 1000000000 + (double) (stop.tv_nsec - start.tv_nsec);
	//printf("num=%ld; time=%f", number, time);
    return NULL;
}

void* broad_thread_func(void* arg)
{
	//Считываем переданые данные
	broad_data* data = (broad_data*) arg;
	int semid = data->semid;
	int procs = data->procs;
	int count = 0;
	
	// Создание UDP сокета -- ловим широковещательный пакет и отвечаем на него, если сообщение правильное
	int broad_sock = socket(PF_INET, SOCK_DGRAM,0);
	struct sockaddr_in addr_broad_rcv;
	bzero(&addr_broad_rcv, sizeof(addr_broad_rcv));
	addr_broad_rcv.sin_family = AF_INET;
	addr_broad_rcv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_broad_rcv.sin_port = htons(PORT_RCV);
	bind( broad_sock, (struct sockaddr*) &addr_broad_rcv, sizeof(addr_broad_rcv) );
	
	//Подготовка принятия и отправки сообщения 
	char rcv_msg[20] = {"\0"};
	int rcv_msg_size = sizeof(rcv_msg);
	struct sockaddr_in addr_broad_answer;
	socklen_t addr_broad_len = sizeof(struct sockaddr_in);
	bzero(&addr_broad_answer, sizeof(addr_broad_answer));
	char ans_msg[20] = {"\0"};
	strcpy( ans_msg, "Yes, I`m here ");
	char str[15];
	sprintf(str, "%d", procs);
	strcat( ans_msg, str);
		
	semoper broading = { 0, -1, 0};
	int iterations = 2;
	while( iterations>0 )
	{
		printf("Trying to start catching broadcasts...\n");
		semop(semid,&broading,1);
		printf("Starting catching broadcasts!!!\n");
		printf("So i`m ready to receive...\n");
		count = recvfrom(broad_sock, (void *) rcv_msg, rcv_msg_size, 0, (struct sockaddr*) &addr_broad_answer, &addr_broad_len);
		printf("Received a message, length %d,  '%s'!\n", count, rcv_msg);
		if( strcmp(rcv_msg, "Are you here?") == 0 )
		{
			count = sendto(broad_sock, ans_msg, strlen(ans_msg), 0, (struct sockaddr*) &addr_broad_answer, addr_broad_len );
			printf("Sent an answer %d\n", count);
		}
		iterations--;
	}
	close(broad_sock);
	return NULL;
}


void* is_alive(void* arg)
{
	alive_data* data = (alive_data*) arg;
	char buffer[10];
	recv(data->client, buffer, 10, 0);
	//Если прошло через это место, то recv получил какое-то значение, значит клиент умер
	int i = 0;
	for(i=0;i< data->procs;i++)
		pthread_kill(data->threads[i], SIGUSR1);
	return NULL;
}
	
