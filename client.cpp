#include "include/common.h"
#include "include/Interpreter.hpp"
#include "include/Client.hpp"

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>

#define MY_PORT		9999
#define MAXBUF		1024


//#define PRN_CASE(val) case val: printf("errno =" #val"\n"); gLogger.log(std::string("errno =" #val"\n");	break;
#define PRN_CASE(val) case val: printf("errno =" #val"\n");	break;

const long partition = 50000;
double a=0.0;
double b=1.0;
int broken = 0;


void main_signal(int signo)
{
	broken = 1;
	printf("Got broken!\n");
}



void prepare_signals() {
	struct sigaction main_act;
	memset(&main_act, 0, sizeof(main_act));
	main_act.sa_handler = main_signal;
	sigfillset(&main_act.sa_mask);
	sigaction(SIGUSR1, &main_act, NULL);
	return;
}




int main( int argc, char** argv )
{
	int sockfd;
	struct sockaddr_in self;

  	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("Socket");
		exit(errno);
	}


	/*---Initialize address/port structure---*/
	bzero(&self, sizeof(self));
	self.sin_family = AF_INET;
	self.sin_port = htons(MY_PORT);
	self.sin_addr.s_addr = INADDR_ANY;

	/*---Assign a port number to the socket---*/
    if ( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 )
	{
		perror("socket--bind");
		exit(errno);
	}

	/*---Make it a "listening socket"---*/
	if ( listen(sockfd, 20) != 0 )
	{
		perror("socket--listen");
		exit(errno);
	}


	char buffer[MAXBUF];
	char exprBuf[MAXBUF];
	char result_string[MAXBUF];
	double int_res = 0.0;
 
	for (;;) {

		int clientfd;
		struct sockaddr_in client_addr;
		int addrlen=sizeof(client_addr);

		/*---accept a connection (creating a data pipe)---*/
		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*) &addrlen);
		printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		memset(result_string, 0, MAXBUF);
		memset(exprBuf, 0, MAXBUF);

		recv(clientfd, exprBuf, MAXBUF, 0);

		printf("%s received\n", exprBuf);

		if (!strcmp(exprBuf, "666")) {
			break;
		}

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
			int_res = client_integrate(exprBuf, a, b);
			//snprintf(result_string, MAXBUF, "%f", re);
			if (broken) {
				strcpy(result_string, "No free servers!");
			} else {
				strcpy(result_string, std::to_string(int_res).c_str());
			}
		//} else if (FUNCTION == root->getNodeType()) {
			
		}
		 else {
			strcpy(result_string, toStringRecursive(root).c_str());
		}

		printf("Result: %s\n", result_string);

		send(clientfd, result_string, strlen(result_string), 0);

		// FILE* outCode = fopen("res.txt", "w");
		// ASSERT(nullptr != outCode);
		// fprintf(outCode, "%s", result_string);
		// ASSERT(EOF != fclose(outCode));

	  
			

			

		// 	/*---Echo back anything sent---*/
		// 	send(clientfd, buffer, recv(clientfd, buffer, MAXBUF, 0), 0);

		// 	/*---Close data connection---*/
		close(clientfd);
		}


	/*---Clean up (should never get here!)---*/
	close(sockfd);
	return 0;
}


	

int prepare_UDP_socket() {

	// Создание UDP сокета 
	int broad_sock = socket(PF_INET, SOCK_DGRAM,0);
	if (broad_sock < 0) ERR_RETURN(ERR_SOCKET);	
	struct sockaddr_in addr_broad_rcv;
	bzero(&addr_broad_rcv, sizeof(addr_broad_rcv));
	SET_ADDR(addr_broad_rcv, AF_INET, htons(0), htonl(INADDR_ANY));
	
	int err = bind( broad_sock, (struct sockaddr*) &addr_broad_rcv, sizeof(addr_broad_rcv) );
	if (err) ERR_RETURN(ERR_BIND);
	printf("broad sock = %d\nbind broad = %d\n", broad_sock, err);

	//Настраиваем сокет на broadcast
	int int_true = 1;
	setsockopt(broad_sock, SOL_SOCKET, SO_BROADCAST, &int_true, sizeof(int_true));

	return broad_sock; 

}


int broad_cast(int broad_sock) {
	//Настраиваем адресс для broadcasta -- 255.255.255.255
	socklen_t addr_len = sizeof(struct sockaddr_in);
	struct sockaddr_in addr_broad_cast;
	bzero(&addr_broad_cast, sizeof(addr_broad_cast));
	SET_ADDR(addr_broad_cast, AF_INET, htons(PORT_SND), htonl(0xffffffff));
	
	//Назначаем отправляемое сообщение
	int snd_msg_size = sizeof(char) * 20;
	void *snd_msg = malloc(snd_msg_size);
	bzero(snd_msg, snd_msg_size);
	strcpy((char*) snd_msg, "Are you here?");
	
	//Делаем broadcast.
	printf("So i`m ready to send...\n");
	int count = sendto(broad_sock, snd_msg, strlen((char*) snd_msg), 0, (struct sockaddr*) &addr_broad_cast, addr_len);
	printf("Sent a message %d : '%s'!\n", count, (char*)snd_msg);

	free(snd_msg);
	return count;
}


int collect_answers(serv_st servers[MAX_NUMB_SERV], int broad_sock) {

	socklen_t addr_len = sizeof(struct sockaddr_in);
	//Сюда будем записывать принимаемые сообщения -- ответы на бродкаст
	char rcv_msg[20] = {"\0"};
	int rcv_msg_size = sizeof(rcv_msg);
	bzero(&rcv_msg, rcv_msg_size);

	fd_set broad_set;
	FD_ZERO(&broad_set);
	FD_SET(broad_sock, &broad_set);
	
	//Делаем select -- смотрим, есть ли чего принять. Ждем три секунды. Если есть -- принимаем, если нет -- через 3 сек select выдаст 0
	struct timeval wait_time;
	wait_time.tv_sec = 1;
	wait_time.tv_usec = 0;
	int current = 0;

	while( select(broad_sock+1, &broad_set, NULL, &broad_set, &wait_time) > 0 && current<MAX_NUMB_SERV)
	{
		recvfrom(broad_sock, (void*)rcv_msg, rcv_msg_size, 0, (struct sockaddr*) &(servers[current].addres), &addr_len);
		if( memcmp(rcv_msg, "Yes, I`m here ", 14) == 0 )
		{
			servers[current].numb_procs = atoi(rcv_msg+14);
			current++;
			printf( "Found client! %s,   %d ...\n", inet_ntoa(servers[current].addres.sin_addr), servers[current].addres.sin_port);
		}
	FD_ZERO(&broad_set);
	FD_SET(broad_sock, &broad_set);
	}
	close(broad_sock);
	printf("Current is %d\n", current);
	if (!current)
	{
		printf("There are no free servers! Try later!\n");
		return 0;
	}
	
	return current;
}


double split_problem_and_solve(serv_st servers[MAX_NUMB_SERV], int current, double a, double b, char* function) {
	//Все сервера записаны в массиве. Распределяем задачу
	pid_t pid = getpid();

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
	while(i < current && !broken)
	{
		printf("Waiting to join %d\n", i);
		pthread_join(tid[i], NULL);
		printf("Joined %d\n", i);
		total_result += *thread_data[i].result;
		free(thread_data[i].result);
		i++;
	}
	if (broken) {
		printf("WUW! Broken!\n");
	}
	free(tid);
	free(thread_data);
	return total_result;
}


double client_integrate(char* function, double a, double b) {
	
	broken = 0;

	prepare_signals();

	// Создание UDP сокета -- отправляем широковещательный пакет и принимаем ответ	
	//Создаем сокет для отправки-приёма broadcast. Порт ставим любой. Биндим его
	int broad_sock = prepare_UDP_socket();
		
	int count = broad_cast(broad_sock);
	if (count < 0) ERR_RETURN(ERR_SENDTO);	

	//Сюда будем писать адресса, с которых пришли ответы. Их надо запомнить
	serv_st servers[MAX_NUMB_SERV];
	bzero( servers, sizeof(serv_st)*MAX_NUMB_SERV);

	int current = collect_answers(servers, broad_sock);	

	if (!current) {
		printf("No free servers! Terminate!\n");
		broken = 1;
		return 0.0;
	}


	double total_result = split_problem_and_solve(servers, current, a, b, function);
	
	if( !broken)
		printf("The result is %lf\n", total_result);	
	else
		printf("One server is broken! Terminated...\n");
	return total_result;	
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
		close(serv_sock);
		printf("Closing!\n");
		//return NULL;
		pthread_exit(PTHREAD_CANCELED);
	}
	//double res = *(thread_data->result);
	printf("Got a result!!! %lf\n", *(thread_data->result));
	close(serv_sock);
	return NULL;
}
