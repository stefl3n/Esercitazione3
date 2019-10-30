#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#define TRUE 1
#define FALSE 0

int main(int argc, char ** argv){
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    struct hostent *remote_host;
    char *input, *pathFile;
    int numLinea, fd,fdc,nread, sock;
    int doExit,doAlright;
    char buf[500];  //c era scritto byte....? comunque char e' un byte
    doExit=FALSE;
    input=(char*)malloc(sizeof(char)*200);
    if(argc!=2){
        printf("errore %s: %s <serverAddr> <serverPortNo>\n",argv[0],argv[0]);
        exit(1);
    }
    
    remote_host = gethostbyname(argv[1]);
    
    if(remote_host==NULL){
        printf("errore %s: gethostbyname error number %d\n",argv[0],h_errno);
        exit(2);
    }
    
    memset((char *)&serverAddr,0,sizeof(struct sockaddr_in));
    //memset((char *)&clientAddr,0,sizeof(struct sockaddr_in)); serve?
    
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ((struct in_addr *)(remote_host->h_addr))->s_addr;
    
    sock=socket(AF_INET,SOCK_STREAM, AF_INET);
    if(sock==-1){
        perror("socket "); 
        exit(2);
    }
    
    //bind implicita
    if(connect(sock,&serverAddr,sizeof(struct sockaddr_in))==-1){
        perror("connect ");
        exit(2);
    }
    
    //while(!exit){
       doAlright=TRUE;
       printf("Inserire path assoluto di un file da caricare e linea da eliminare ( pathFile numLinea) Ctrl+d per terminare\n");
       
       //if(scanf("%s",input)==1){//TODO: testare se funziona la Ctrl+d
         //  if(input[0]==EOF) exit=TRUE;
           //else printf("input errato\n");
       //}
       
       pathFile = strtok(input, " ");
       if(pathFile == NULL){
           printf("errore in input, inserire: pathFile numLinea\n");
           doAlright=FALSE;
       }
       
       if(doAlright && (fd=open(pathFile,O_RDONLY))==-1){
           printf("fallimento nell'apertura del file %s\n",pathFile);
           doAlright=FALSE;
       }
       
       if(doAlright){
           numLinea = atoi(strtok(input, " "));//TODO:usare funzione di conversione che consenta controllo errore
       }
       
       //protocollo programma
       //if(alright && !exit){
       if(doAlright){
           *buf=numLinea;
           
           //e se si creasse un processo che mentre il padre invia il file intanto il figlio inizia a ricevere dal server?
           send(sock, buf, sizeof(int), 0);//ho messo 0 come flag ma cosi e' equivalente alla write
           
           
           //invio contenuto file al server a gruppi di 500byte
           while((nread=read(fd,buf,500)) > 0){
               send(sock,buf,nread, 0);
           }
           shutdown(sock,SHUT_WR);
           
           //creazione file senza linea
           fdc=open(strcat(pathFile,"noLinea"),O_CREAT | O_WRONLY);
           while((nread=read(sock, buf, 500))>0){
               write(fdc,buf,nread);
           }
           close(sock);
           close(fdc);
           close(fd);
       }
       //}
    //}
    
}
