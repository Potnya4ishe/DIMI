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
//#include "common.h"
#include <string>
#include "include/Interpreter.hpp"
//#include <Logger.hpp>

#define PORT_SND 38199
#define MAX_NUMB_SERV 100

//#define PRN_CASE(val) case val: printf("errno =" #val"\n"); gLogger.log(std::string("errno =" #val"\n");	break;
#define PRN_CASE(val) case val: printf("errno =" #val"\n");	break;


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
	char function[64];
} serv_data;

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

const long partition = 50000;
double a=0.0;
double b=1.0;
int broken = 0;

void main_signal(int signo)
{
	broken = 1;
}

int client_integrate(char* function, double a, double b);

void prepare_signals() {
	struct sigaction main_act;
	memset(&main_act, 0, sizeof(main_act));
	main_act.sa_handler = main_signal;
	sigfillset(&main_act.sa_mask);
	sigaction(SIGUSR1, &main_act, NULL);
}




//int main_client(const char* function, double a, double b)
int main( int argc, char** argv )
{
	//char* expr = "d(x*x)";
	char exprBuf[64];
	scanf("%s", exprBuf);

	Interpreter interp;
	ASTree* tree = interp.run(exprBuf);
	ASTnode* root = (ASTnode*)tree->getRootNode();

	if (FUNCTION == root->getNodeType() && 
		"int" == ((Function*)root)->getName()) {
		double a = 0.0;
		double b = 1.0;
		std::string subTreeStr = toStringRecursive(((Function*)root)->getArgument());
		const char* tmpCharStr = subTreeStr.c_str();
		strcpy(exprBuf, tmpCharStr);
		client_integrate(exprBuf, a, b);
	}

	printf("%s\n", root->toString().c_str());

}

	
int client_integrate(char* function, double a, double b) {

	pid_t pid = getpid();

	socklen_t addr_len = sizeof(struct sockaddr_in);
	int count = 0;
	
	// Создание UDP сокета -- отправляем широковещательный пакет и принимаем ответ
	
	//Создаем сокет для отправки-приёма broadcast. Порт ставим любой. Биндим его
	int broad_sock = socket(PF_INET, SOCK_DGRAM,0);
	struct sockaddr_in addr_broad_rcv;
	bzero(&addr_broad_rcv, sizeof(addr_broad_rcv));
	addr_broad_rcv.sin_family = AF_INET;
	addr_broad_rcv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_broad_rcv.sin_port = htons(0);
	count = bind( broad_sock, (struct sockaddr*) &addr_broad_rcv, sizeof(addr_broad_rcv) );
	
	printf("broad sock = %d\nbind broad = %d\n", broad_sock, count);
	//gLogger.log(std::string("broad sock = ") + std::to_string(long(broad_sock)) + std::string("bind broad = ") + std::to_string(long(count)));
	
	//Настраиваем сокет на broadcast
	int int_true = 1;
	setsockopt(broad_sock, SOL_SOCKET, SO_BROADCAST, &int_true, sizeof(int_true));
	
	//Настраиваем адресс для broadcasta -- 255.255.255.255
	struct sockaddr_in addr_broad_cast;
	bzero(&addr_broad_cast, sizeof(addr_broad_cast));
	addr_broad_cast.sin_family = AF_INET;
	addr_broad_cast.sin_addr.s_addr = htonl(0xffffffff);
	addr_broad_cast.sin_port = htons(PORT_SND);
	
	//Назначаем отправляемое сообщение
	int snd_msg_size = sizeof(char) * 20;
	void *snd_msg = malloc(snd_msg_size);
	bzero(snd_msg, snd_msg_size);
	strcpy((char*) snd_msg, "Are you here?");
	
	//Делаем broadcast.
	printf("So i`m ready to send...\n");
	count = sendto(broad_sock, snd_msg, strlen((char*) snd_msg), 0, (struct sockaddr*) &addr_broad_cast, addr_len);
	printf("Sent a message %d : '%s'!\n", count, (char*)snd_msg);
	
	//Сюда будем записывать принимаемые сообщения -- ответы на бродкаст
	char rcv_msg[20] = {"\0"};
	int rcv_msg_size = sizeof(rcv_msg);
	bzero(&rcv_msg, rcv_msg_size);
	
	//Сюда будем писать адресса, с которых пришли ответы. Их надо запомнить
	serv_st servers[MAX_NUMB_SERV];
	bzero( servers, sizeof(serv_st)*MAX_NUMB_SERV);

	//printf("So i`m ready to receive...\n");
	fd_set broad_set;
	FD_ZERO(&broad_set);
	FD_SET(broad_sock, &broad_set);
	
	//Делаем select -- смотрим, есть ли чего принять. Ждем три секунды. Если есть -- принимаем, если нет -- через 3 сек select выдаст 0
	struct timeval wait_time;
	wait_time.tv_sec = 3;
	wait_time.tv_usec = 0;
	//int count=0;
	int current = 0;
	while( select(broad_sock+1, &broad_set, NULL, &broad_set, &wait_time) > 0 && current<MAX_NUMB_SERV)
	{
		recvfrom(broad_sock, (void*)rcv_msg, rcv_msg_size, 0, (struct sockaddr*) &servers[current].addres, &addr_len);
		if( memcmp(rcv_msg, "Yes, I`m here ", 14) == 0 )
		{
			servers[current].numb_procs = atoi(rcv_msg+14);
			current++;
			printf( "Found client! %s ...\n", inet_ntoa(servers[current].addres.sin_addr));
		}
	FD_ZERO(&broad_set);
	FD_SET(broad_sock, &broad_set);
	}
	close(broad_sock);
	free(snd_msg);
	printf("Current is %d\n", current);
	if (!current)
	{
		printf("There are no free servers! Try later!\n");
		return 0;
	}
	//Все сервера записаны в массиве. Распределяем задачу
	pthread_t* tid = (pthread_t*) malloc (current * sizeof(pthread_t));
	serv_thread_data* thread_data = (serv_thread_data*) malloc(current *sizeof(serv_thread_data));
	int i= 0;
	int total_procs = 0;
	for(i = 0; i<current; i++)
		total_procs+=servers[i].numb_procs;
	printf("Total procs : %d\n", total_procs);
	long parts_per_proc = (long) partition/total_procs;
	double step = (b-a)/total_procs;
	double current_position = a;
	printf("step = %lf\n", step);
	for(i = 0; i < current; i++)
	{
		thread_data[i].server = servers[i].addres;
		thread_data[i].partition = parts_per_proc*servers[i].numb_procs;
		thread_data[i].a = current_position;
		current_position += step*servers[i].numb_procs;
		thread_data[i].b = current_position;
		thread_data[i].result = (double*) malloc (sizeof(double));
		thread_data[i].pid = pid;
		printf("HUI1 %s\n", function);
		strcpy(thread_data[i].function, function);
		printf("HUI2\n");
		pthread_create(&tid[i], NULL, (work_with_server), &thread_data[i]);
	}
	double total_result;
	i = 0;
	while(i<current && !broken)
	{
		pthread_join(tid[i], NULL);
		total_result += *thread_data[i].result;
		free(thread_data[i].result);
		i++;
	}
	free(tid);
	free(thread_data);
	if( !broken)
		printf("The result is %lf\n", total_result);	
	else
		printf("One server is broken! Terminated...\n");
	return 0;	
}





void* work_with_server(void* arg_box)
{
	socklen_t addr_len = sizeof(struct sockaddr_in);
	int count = 0;
	
	serv_thread_data* thread_data = (serv_thread_data*) arg_box;
	
	//Создаем сокет для общения с сервером
	int serv_sock = socket(PF_INET, SOCK_STREAM,0);
	struct sockaddr_in addr_client;
	bzero(&addr_client, sizeof(addr_client));
	addr_client.sin_family = AF_INET;
	addr_client.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_client.sin_port = htons(0);
	count = bind(serv_sock, (struct sockaddr*) &addr_client, addr_len);
	
	printf("serv sock = %d\nbind serv = %d\n", serv_sock, count);
	
	//Конектимся к серверу
	count = connect(serv_sock, (struct sockaddr*) &(thread_data->server), addr_len);
	if(count <0 )
	{
		printf("Problems with CONNECT! connect = %d\n", count);
		switch(errno){
				PRN_CASE(EAGAIN)
				PRN_CASE(EBADF)
				PRN_CASE(ECONNREFUSED)
				PRN_CASE(ENOTSOCK)
				PRN_CASE(EINTR)
				PRN_CASE(EFAULT)
				PRN_CASE(EINVAL)
				PRN_CASE(EISCONN)
				PRN_CASE(ETIMEDOUT)
				PRN_CASE(ENETUNREACH)
				PRN_CASE(EADDRINUSE)
				PRN_CASE(EINPROGRESS)
				PRN_CASE(EALREADY)
				PRN_CASE(EAFNOSUPPORT)
				PRN_CASE(EACCES)
				PRN_CASE(EPERM)
			}	
			kill(thread_data->pid, SIGUSR1);
			pthread_exit(PTHREAD_CANCELED);
	}
	printf( "connect = %d\n", count);
	
	//Заполняем отправляемую структуру и отправляем
	serv_data data;
	data.a = thread_data->a;
	data.b = thread_data->b;
	data.partition = thread_data->partition;
	strcpy(data.function, thread_data->function);
	printf("I'm thread with a = %lf, b = %lf, partition = %ld. Sending...\n ", data.a, data.b, data.partition);
	count = send(serv_sock, &data, sizeof(data), 0);
	if(count <= 0 )
	{
		printf("Broken server!!! Can't send!\n");
		kill(thread_data->pid, SIGUSR1);
		pthread_exit(PTHREAD_CANCELED);
	}
	printf("Sent %d. Waiting for answer...(a = %lf)\n", count, data.a);
	
	//Получаем ответ
	count = recv(serv_sock, thread_data->result, sizeof(double), 0);
	printf("Got %d answer\n", count);
	if(count <= 0 )
	{
		printf("Broken server!!! Can't receive!\n");
		kill(thread_data->pid, SIGUSR1);
		pthread_exit(PTHREAD_CANCELED);
	}
	//double res = *(thread_data->result);
	printf("Got a result!!! %lf\n", *(thread_data->result));
	close(serv_sock);
	return NULL;
}
