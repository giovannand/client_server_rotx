#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h> 
#include <inttypes.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<unistd.h> 

void logexit(const char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

struct dados {
	int sock;
	struct sockaddr_in addr;
};

void dencryptcifraDeCesar(char *message,int size, int chave){

	int i=0;

	for (i = 0; i < size; ++i) {

		if( (message[i]-chave) >=97){
			message[i] = message[i]-chave;
		}else{
			message[i] = 123 - (97-(message[i]-chave)) ;
		}

	}
	printf("%s\n",message);

}


void * client_thread(void *param) {
	pthread_t tid = pthread_self();
	struct dados *dd = param;
	int r = dd->sock;

	char ipcliente[512];
	inet_ntop(AF_INET, &(dd->addr.sin_addr),
			ipcliente, 512);

	// printf("conexao de %s %d\n", ipcliente,(int)ntohs(dd->addr.sin_port));

	// printf("thread %x esperando receber\n",(unsigned int)tid);

    //---------------------------------------------------
	    char buf[512];
		char *message; 
		uint32_t size_message;
		uint32_t code;
		sleep(20);
		size_t return_status = recv(r, &size_message, sizeof(size_message),0);
		if (return_status <= 0) {
		// 	fprintf(stdout, "Received %d bytes. Size message = %" PRIu32 "\n", (int)return_status,ntohl(size_message));
		// }else{
			logexit("recv");
		}
		size_t return_status2 = recv(r, &code, sizeof(code),0);
		if (return_status2 <= 0) {
		// 	fprintf(stdout, "Received %d bytes. Code = %" PRIu32 "\n", (int)return_status,ntohl(code));
		// }else {
			logexit("recv");
		}
		message =   (char *)malloc((size_message+1) * sizeof(char)); 
		size_t c = recv(r, message, size_message, 0);
		if (c <= 0) {
		// 	printf("Received %d bytes. Message: %s \n", (int)c,message);
		// }else { 
			logexit("recv");
		}
		dencryptcifraDeCesar(message,strlen(message), ntohl(code));
//---------------------------------------------------

	// sprintf(buf, "seu IP eh %s %d\n", ipcliente,		(int)ntohs(dd->addr.sin_port));
    // printf("\nEnviando %s\n", buf);
	
    send(r, message, strlen(message), 0);
    //  printf("\nEnviou\n");

    close(r);

	pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	int port;
	port =  atoi(argv[1]);

	setvbuf(stdout, NULL, _IONBF, 0);
	int s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == -1) logexit("socket");

	struct timeval time_val;
	time_val.tv_sec = 15;
	time_val.tv_usec = 0;
	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&time_val,sizeof(struct timeval)) == -1)
		printf("problema pra setar time out");
	if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&time_val,sizeof(struct timeval)) == -1)
		printf("problema pra setar time out");


	struct in_addr inaddr;
	inet_pton(AF_INET, "127.0.0.1", &inaddr);

	struct sockaddr_in addr;
	struct sockaddr *addrptr = (struct sockaddr *)&addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = inaddr;

	if(bind(s, addrptr, sizeof(struct sockaddr_in)))
		logexit("bind");

	if(listen(s, 10)) logexit("listen");
	// printf("esperando conexao\n");

	while(1) {
		struct sockaddr_in raddr;
		struct sockaddr *raddrptr =
			(struct sockaddr *)&raddr;
		socklen_t rlen = sizeof(struct sockaddr_in);

		int r = accept(s, raddrptr, &rlen);
		if(r == -1) logexit("accept");

		struct dados *dd = malloc(sizeof(*dd));
		if(!dd) logexit("malloc");
		dd->sock = r;
		dd->addr = raddr;

		pthread_t tid;
		pthread_create(&tid, NULL, client_thread, dd);
	}

	exit(EXIT_SUCCESS);
}




