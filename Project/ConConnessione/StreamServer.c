#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define true 1
#define false 0

/********************************************************/
void gestore(int signo){
  int stato;
  printf("esecuzione gestore di SIGCHLD\n");
  wait(&stato);
}
/********************************************************/

int main(int argc, char **argv)
{
	int  listen_sd, conn_sd;
	int port, len, num, nlinea, i, nread;
	const int on = 1;
    char c;
	struct sockaddr_in cliaddr, servaddr;
	struct hostent *host;

	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=2){
		printf("Error: %s port\n", argv[0]);
		exit(1);
	}
	else{
		num=0;
		while( argv[1][num]!= '\0' ){
			if( (argv[1][num] < '0') || (argv[1][num] > '9') ){
				printf("Secondo argomento non intero\n");
				exit(2);
			}
			num++;
		} 	
		port = atoi(argv[1]);
		if (port < 1024 || port > 65535){
			printf("Error: %s invalid port %d\n", argv[0],port);
			printf("1024 <= port <= 65535\n");
			exit(2);  	
		}

	}

	/* INIZIALIZZAZIONE INDIRIZZO SERVER ----------------------------------------- */
	memset ((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	/* CREAZIONE E SETTAGGI SOCKET D'ASCOLTO --------------------------------------- */
	listen_sd=socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sd <0)
	{perror("creazione socket "); exit(1);}
	printf("Server: creata la socket d'ascolto per le richieste di ordinamento, sd=%d\n", listen_sd);

	if(setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0)
	{perror("set opzioni socket d'ascolto"); exit(1);}
	printf("Server: set opzioni socket d'ascolto ok\n");

	if(bind(listen_sd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0)
	{perror("bind socket d'ascolto"); exit(1);}
	printf("Server: bind socket d'ascolto ok\n");

	if (listen(listen_sd, 5)<0)
	{perror("listen"); exit(1);}
	printf("Server: listen ok\n");

	signal(SIGCHLD, gestore);
	
	while(true){
		len=sizeof(cliaddr);
		if((conn_sd=accept(listen_sd,(struct sockaddr *)&cliaddr,&len))<0){
			if (errno==EINTR){
				perror("Interruzione accept ");
				continue;
			}
			else exit(1);
		}
		
		if(fork()==0){
			close(listen_sd);
			
			host = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
			if(host==NULL){
				printf("client host information not found\n");
				continue;
			}
			else
				printf("Server (figlio %d): host client è: %s \n", getpid(), host-> h_name);
			
			printf("Server(figlio %d): filtro il file", getpid());
			
			if((read(conn_sd,&nlinea,sizeof(int)))<0){
				perror("Errore lettura n°linea ");
				exit(1);
			}
				
            printf("nlinea=%d\n",nlinea);
				i=1;
			while((nread=read(conn_sd,&c,sizeof(char)))!=0){
				if(nread<0){
					close(conn_sd);
					perror("Errore lettura ");
					exit(2);
				}
				if(i!=nlinea){
					if((write(conn_sd,&c,sizeof(char)))<0){
						perror("Errore scrittura ");
						exit(2);
					}
				}
				
				if(c=='\n')
					i++;
			}
			close(conn_sd);
		}
		close(conn_sd);
	}
}
