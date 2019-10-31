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
#include <sys/time.h>

int main(int argc, char **argv){
	struct hostent *host;
	struct sockaddr_in clientaddr, servaddr;
	int port, sd,byteletti, risposta, temp,len,cont;
	struct timeval start, stop;
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

	printf("Inserisci il file che vuoi analizzare (EOF per terminare)\n");
	
	while((scanf("%s", req)!=EOF)){
		len = sizeof(servaddr);
		
		gettimeofday(&start, NULL);
		if((sendto(sd, &req, sizeof(req)  , 0, (struct sockaddr *)&servaddr, len))<0){
			perror("sendto ");
			exit(1);
		}
		
		if((recvfrom(sd, &temp, sizeof(int), 0, (struct sockaddr *)&servaddr, &len))<0){
			perror("recvfrom ");
			exit(1);
		}
		gettimeofday(&stop, NULL);
		printf("Risposta ricevuta [%f] -> ", (double) (stop.tv_sec - start.tv_sec) * 1000 + (double) (stop.tv_usec - start.tv_usec) / 1000);
		
		if(temp == -1){
			printf("ERRORE: File non esistente (o errore nella lettura)\n");
		}else 
			printf("SUCCESSO: La parola piu lunga misura %d lettere\n", temp);
			
		printf("\nInserire prossimo file (EOF per terminare)\n");
	}
	close(sd);
	return 0;
}
	

