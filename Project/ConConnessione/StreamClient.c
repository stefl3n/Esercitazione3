#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char ** argv){
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    
    struct hostent remote_host;
    if(argc!=2){
        printf("errore %s: %s <serverAddr> <serverPortNo>\n");
        exit(1);
    }
    
    memset((char *)&serverAddr,0,sizeof(struct sockaddr_in));
    
    remote_host = gethostbyname(argv[1]);//check errore
    
    if(remote_host==NULL){
        printf("errore %s: gethostbyname gone wrong");
        exit(1);
    }
    clientAddr
    
    serverAddr.sin_port=htons(atoi(argv[2]));//check errore
    serverAddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr = ((struct in_addr *)(remote_host->h_addr))->s_addr;
}
