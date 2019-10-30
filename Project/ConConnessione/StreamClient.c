#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/wait.h>
#include <fcntl.h>
#define TRUE 1
#define FALSE 0

int main(int argc, char ** argv){
    struct sockaddr_in serverAddr;
    struct hostent *remote_host;
    char input[270], *pathFile;
    int numLinea, fd,fdc,nread, sock, p, stat;
    int doExit,doAlright;
    char buf[500];
    char rec_c;
    char *next_read;//deb
    doExit=FALSE;
    
    if(argc!=3){
        printf("errore %s: <serverAddr> <serverPortNo>\n",argv[0]);
        exit(1);
    }
    
    remote_host = gethostbyname(argv[1]);
    
    if(remote_host==NULL){
        printf("errore %s: gethostbyname error number %d\n",argv[0],h_errno);
        exit(2);
    }
    
    memset((char *)&serverAddr,0,sizeof(struct sockaddr_in));
    
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ((struct in_addr *)(remote_host->h_addr))->s_addr;
    
    sock=socket(AF_INET,SOCK_STREAM, 0);
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
      printf("Inserire path assoluto di un file da caricare e linea da eliminare ( pathFile numLinea)\n");
       
       //if(scanf("%s",input)==1){
         //  if(input[0]==EOF) exit=TRUE;
           //else printf("input errato\n");
       //}
       scanf("%s",input);
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
       if(doAlright){//BUG invia 0 a prescindere :( forse strtok canna cose o io non la so usare
           printf("sto per inviare il numero numlinea=%d\n",numLinea);
           write(sock, &numLinea, sizeof(int));
           
           p=fork();//creo figlio per leggere dal server
           
           if(p==0){
                //invio contenuto file al server a gruppi di 500byte
                while((nread=read(fd,buf,500)) > 0){
                    write(sock,buf,nread);
                }
                shutdown(sock,SHUT_WR);
                wait(&stat);//
                printf("ricezione da server terminata con stato %d\n",stat);
           }
           else{
                //creazione file senza linea
                fdc=open(strcat(pathFile,"noLinea"),O_CREAT | O_WRONLY);
                printf("file creato fdc=%d, inizio a leggere\n",fdc);
                if(fdc!=-1){
                    
                //BUG viene creato file non apribile.
                while((nread=read(sock, &rec_c, sizeof(char)))>0){
                    printf("%c",rec_c);//debug
                    write(fdc,&rec_c,nread);
                }
                
                shutdown(sock,SHUT_RD);
                close(fdc);
                exit(0);
                }
                else{
                    printf("errore creazione file\n");
                    //shutdown(sock, SHUT_RD);
                    exit(1);
                }
           }
           close(sock);
           close(fd);
           printf("padre (client) esco\n");
       }
       //}
    //}
    
}
