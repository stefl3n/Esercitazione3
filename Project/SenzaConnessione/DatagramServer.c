#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

/********************************************************/
void gestore(int signo){
  int stato;
  printf("esecuzione gestore di SIGCHLD\n");
  wait(&stato);
}
/********************************************************/

int main (int argc, char ** argv){
	int sd, port, len, pid, fd, maxlen, i;
	char[256] fileName;
	char c;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	struct hostent *clienthost;
	
	
	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=2){
		printf("Error: %s port\n", argv[0]);
		exit(1);
	}
	else{
		num1=0;
		while( argv[1][num1]!= '\0' ){
			if((argv[1][num1] < '0') || (argv[1][num1] > '9')){
				printf("Secondo argomento non intero\n");
				printf("Error: %s port\n", argv[0]);
				exit(2);
			}
			num1++;
		}  	
	  	port = atoi(argv[1]);
  		if (port < 1024 || port > 65535){
		      printf("Error: %s port\n", argv[0]);
		      printf("1024 <= port <= 65535\n");
		      exit(2);  	
  		}
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
			perror("recvfrom "); continue;
		}
		
		printf("File richiesto: %s", fileName);
		
		if(fd = open(fileName, O_RDNLY)<0){
			perror("Errore apertura file ");
			maxlen=-1;
			if(sendto(sd, &maxlen, sizeof(int), 0, (struct sockaddr *)&cliaddr, len)<0)
				perror("sendto ");
			continue;
		}
		
		clienthost=gethostbyaddr( (char *) &cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
		if(clienthost==NULL){
			printf("client host information not found\n");
		}
		else
			printf("Richiesta da Cliente: %s %i", clienthost->h_nam, (unsigned)ntohs(cliaddr.sin_port));
		
		signal(SIGCHLD, gestore);
		
		pid = fork();
		if(pid==0){
			/* Da aggiungere se si vuole 2 cose : 
			 * 1)Stampare la parola
			 * 2)Dare la possibilità di cambiare delimitatori
			 * 3)Esecuzione sequenziale
			 */
			i=0;
			maxlen=0;
			while(read(fd,&c,sizeof(char))>0 && c!=EOF){
				if(c==' ' || c=='\n'){
					if(maxlen<i)
						maxlen=i;
					i=0;
				}else 
					i++;
			}
			close(fd);
			
			if((sendto(sd ,&maxlen ,sizeof(int) ,0 ,(struct sockaddr*)&cliaddr ,len)<0)
				perror("sendto");
			close(sd);
			
		}
		
		close(fd);
		close(sd);
	}
}