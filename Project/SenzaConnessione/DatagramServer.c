#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#define true 1
#define false 0
#define DIM_FILE 30
/********************************************************/
void gestore(int signo){
  int stato;
  printf("esecuzione gestore di SIGCHLD\n");
  wait(&stato);
}
/********************************************************/

int contaPiuLunga(int sd, int delen, int fd, char * delimitatori){
	int i=0, j, trovato, nread, maxlen=0;
	char c;
	do{
		nread=read(fd,&c,sizeof(char));
			
		if(nread<0){
			perror("Errore lettura: ");
			close(fd);
			return -1;
		}
	
		trovato = false;
		for(j=0; j<delen && !trovato; j++){
			if(c==delimitatori[j] || nread==0 || c==' ' || c=='\n'){
					trovato = true;
					if(maxlen<i)
						maxlen=i;
					i=0;
			}
		}
				
		if(!trovato)
			i++;
	}while(nread!=0);
	
	close(fd);
	return maxlen;
}

int main (int argc, char ** argv){
	const int on = 1;
	int port, sd, fd, len, delen, maxlen, num1, dim=0;
	char fileName[256];
	char * delimitatori;
	struct sockaddr_in cliaddr, servaddr;
	struct hostent *clienthost;
	
	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=2 && argc!=3){
		printf("Error: %s port [delimitatori]\n", argv[0]);
		exit(1);
	}
	else{
		num1=0;
		while( argv[1][num1]!= '\0' ){
			if((argv[1][num1] < '0') || (argv[1][num1] > '9')){
				printf("Secondo argomento non intero\n");
				printf("Error: %s port [delimitatori]\n", argv[0]);
				exit(2);
			}
			num1++;
		}  	
	  	port = atoi(argv[1]);
  		if (port < 1024 || port > 65535){
		      printf("Error: %s port [delimitatori]\n", argv[0]);
		      printf("1024 <= port <= 65535\n");
		      exit(2); 
  		}
	}
	
	if(argc==3){
		delimitatori = argv[2];
		delen = strlen(delimitatori);
	}
	
	/* INIZIALIZZAZIONE INDIRIZZO SERVER ---------------------------------- */
	memset ((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;  
	servaddr.sin_port = htons(port);  
	
	/* CREAZIONE, SETAGGIO OPZIONI E CONNESSIONE SOCKET -------------------- */
	sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd <0){perror("creazione socket "); exit(1);}
	printf("Server: creata la socket, sd=%d\n", sd);

	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0)
	{perror("set opzioni socket "); exit(1);}
	printf("Server: set opzioni socket ok\n");

	if(bind(sd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0)
	{perror("bind socket "); exit(1);}
	printf("Server: bind socket ok\n");
	
	/* CICLO RICEZIONE RICHIESTE ----------------------------------------------------------------------- */
	while(true){
		len=sizeof(struct sockaddr_in);
		if(recvfrom(sd, fileName, sizeof(fileName), 0, (struct sockaddr *)&cliaddr, &len)<0){
			perror("recvfrom ");
			return 0;
		}
		
		printf("File richiesto: %s\n", fileName);
		
		if((fd = open(fileName, O_RDONLY))<0){
			perror("Errore apertura file ");
			maxlen=-1;
			if((sendto(sd ,&maxlen ,sizeof(int) ,0 ,(struct sockaddr*)&cliaddr ,len)<0))
				perror("sendto");
			continue;
		}
		
		clienthost=gethostbyaddr( (char *) &cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
		if(clienthost==NULL){
			printf("client host information not found\n");
		}
		else
			printf("Richiesta da Cliente: %s %i\n", clienthost->h_name, (unsigned)ntohs(cliaddr.sin_port));
		
		
		if(dim>DIM_FILE){
			signal(SIGCHLD, gestore);
			if(fork()==0)
				maxlen = contaPiuLunga(sd, delen, fd, delimitatori);
		}else
			maxlen = contaPiuLunga(sd, delen, fd, delimitatori);
		
	
		if((sendto(sd ,&maxlen ,sizeof(int) ,0 ,(struct sockaddr*)&cliaddr ,len)<0))
				perror("sendto");
	}
}