/*
 * Client.c
 *
 *  Created on: Oct 28, 2019
 *      Author: root
 */
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>


	int main(int argc, char **argv){

	struct hostent *host;
	struct sockaddr_in clientaddr, servaddr;
	int port, sd,byteletti;
	char  req[256];
	char c = 's';

	if (argc != 3){

		printf("Errore argomenti");
		exit(1);
	}

	memset((char *)&clientaddr, 0, sizeof(struct sockaddr_in));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = INADDR_ANY;
	clientaddr.sin_port = 0;

	memset((char*)&servaddr, 0 , sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	host = gethostbyname(argv[1]);
	port = atoi(argv[2]);
	printf("%i \n", port);
	if(port < 1024 || port > 65535){
		printf("Errore porta"),
		exit(1);
	}

	if(host == NULL){
		exit(2);


	}else{

		servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
			servaddr.sin_port = htons(port);

			sd = socket(AF_INET, SOCK_DGRAM, 0);

	}
	if(sd < 0){
		printf("Errore socket");
		exit(1);
	}

	if(bind(sd,(struct sockaddr *) &clientaddr, sizeof(clientaddr)) < 0){

		printf("Errore bind");
		exit(1);
	}


	printf("%i \n",sizeof(req));

	while(c == 's'){

		int risposta, temp,len;

		printf("Inserisci il file che vuoi analizzare \n");
		scanf("%s", req);
		printf("%s",req);
		getchar();
		printf("%i \n",sizeof(req));
		if(strcmp(req," ") == 0){

			printf("Nome file non può essere vuoto \n");

		}else {
			len = sizeof(servaddr);
			byteletti = sendto(sd, &req, sizeof(req), 0, (struct sockaddr *)&servaddr, len);
			printf("Nome inviato\n e byte inviati: %i \n", byteletti);
			recvfrom(sd, &temp, sizeof(int), 0, (struct sockaddr *)&servaddr, &len);
			printf("risposta ricevuta \n");
			risposta = ntohl(temp);
			printf("La parola piu lunga misura %d lettere \n", risposta);
			printf("Vuoi continuare ? s/n \n");
			scanf("%c", &c);


		}



	}
	close(sd);
	return 0;
}
