

/*
 ============================================================================
 Name        : client.c
 Author      : Giovanna Neves Damasceno
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <regex.h>
#include<unistd.h>

#define BUFSZ 1024

void logexit(const char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}



void encryptCifraDeCesar(char *message,int size, int chave){

	int i=0;

	for (i = 0; i < size; ++i) {

		if( (message[i]+chave) <=122){
			message[i] = message[i]+chave;
		}else{
			message[i] = ((message[i]+chave)-122) +96;
		}

	}
//	printf("\nmessage %s",message);

}

int verifyInput(char *message){
	int i=0;

	for (i = 0; i < strlen(message); ++i) {
		if(!(message[i] <=122 && message[i]>=97)){
			printf("Input not valid");
			return -1;
		}
	}
	return 0;

}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
///MUDA A DROGA DA ORDEM DOS PARAMETROS MENINA NAO ESQUECE DISSO PELO AMOR DA DEUSA

	/*
	 * Inicializando variaveis
	 */
	int port;
	int verification = -1;
	int s,code_int;
	uint32_t size_message;
	uint32_t code;
	ssize_t count,count2,count3;
	char *message; 	/*APENAS [A-Z] - VER SE C IMPLEMENTA REGEX*/

	//Preparando variaveis

	code_int = atoi(argv[4]);
	message = (char *)malloc(strlen(argv[3]) * sizeof(char));
	strcpy(message, argv[3]);

	//Atribuindo valores

	size_message = htonl(strlen(message));
	//code = ( (*(uint32_t*)&code_int ) );
	code = htonl(atoi(argv[4]));
	port =  atoi(argv[2]);
	//printf("%" PRIu32 "\n",code);



	verification = verifyInput(message);

	if (verification != 0)
			return EXIT_FAILURE;

	encryptCifraDeCesar(message,strlen(message), atoi(argv[4]));

	/*
	 * Formatando endereço de conexao do socket
	 */
	struct in_addr inaddr;
	inet_pton(AF_INET, argv[1],&inaddr);

	struct sockaddr_in addr;
	struct sockaddr *addrptr = (struct sockaddr *)&addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = inaddr;

	/*
	 * Configurando e conectando via socket
	 */


	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == -1) logexit("socket");

	struct timeval time_val;
	time_val.tv_sec = 15;
	time_val.tv_usec = 0;

	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&time_val,sizeof(struct timeval)) == -1)
		printf("problema pra setar time out");
	if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&time_val,sizeof(struct timeval)) == -1)
		printf("problema pra setar time out");

	if(connect(s, addrptr, sizeof(struct sockaddr_in)))
		logexit("connect");


	/*
	 * Enviando mensagens para o servidor
	 * uint32_t size_message : tamanho do string
	 * char * 	message		 : string de entrada codificado
	 * uint32_t code		 : chave para decifrar entrada codificada
	 */

	count 	= send(s,&(size_message), sizeof(size_message), 0);
	if(count != sizeof(size_message))
		logexit("send");

	count3 	= send(s,&(code), sizeof(code), 0);
	if(count3 != sizeof(code))
		logexit("send");

	count2 	= send(s, message, strlen(message), 0);
	if(count2 != strlen(message))
		logexit("send");



	/*
	 * Recebando mensagens do servidor
	 */
	char buf[512];
	memset(buf, 0, 512);
	unsigned total = 0;
	while(1) {
		count = recv(s, buf+total, 512, 0);
		if(count == 0) break;
		total += count;
	}

//	printf("received %d bytes\n", (int)total);
	puts(buf);


	return EXIT_SUCCESS;
}
