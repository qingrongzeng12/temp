
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define MAXLINE 4096
#define MAXCONN 30

int checkAvailablefd(int fdarray[], int fdflag[], int fdm);
void setFd(int fdarray[], int fdflag[], int fdm, fd_set* readfds, fd_set* writefds, fd_set* exceptfds);

int get_index(int fdm);
int get_index(int fdm){
  return fdm+1;
}

int socket_client();
int select_socket_server();

//server
int main(int argc, char** argv) 
{
	select_socket_server();
}

int select_socket_server() {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[4096];
    int n;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  
    
    servaddr.sin_port = htons(12345);

    if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof (servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    if (listen(listenfd, MAXCONN) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    
    
    
    
    fd_set fdset;
    
    int fd_A[MAXCONN];    //fd array
    int fd_C[MAXCONN];    //copy of fd array, point out exist
    int fdi;              //max index of current fd array 
    
    int fd_array[MAXCONN];          //fd array
    int fd_flag[MAXCONN] = {0};     //copy of fd array, point out exist
    int fdm = 0;                    //max index of current fd array 
    int fdindexa;                   //current available fd index
    
    int maxfd = listenfd;  //set default max fd
    fd_set readfds, writefds, exceptfds;
    struct timeval tv;
    int ret;
    int i;
    
    fd_array[0] = listenfd;
    fd_flag[0] = 1;
    fdm = 1;
        
    //inet_ntoa
    printf("server is %s, port is %d\n", inet_ntoa(servaddr.sin_addr.s_addr), ntohs(servaddr.sin_port) );
    printf("======waiting for client's request======\n");
    while(1){
        
        FD_ZERO(&readfds); //init fds
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        setFd(fd_array, fd_flag, fdm, &readfds, &writefds, &exceptfds);
        tv.tv_sec = 0; //set timeout
        tv.tv_usec = 0;
        
        ret = select(maxfd+1, &readfds, &writefds, &exceptfds, &tv);
        if (ret <= 0){
          continue;
        }
        //ret > 0
        if (FD_ISSET(listenfd, &readfds)){
          //here check socket overflow
          fdindexa = checkAvailablefd(fd_array, fd_flag, fdm);
          if (fdindexa == 0) {
            printf("There's no available fd\n");
            continue;
          }
        
          if ((connfd = accept(listenfd, (struct sockaddr*) NULL, NULL)) == -1) {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            continue;
          }
          printf("Success accept socket %d\n", connfd);
          //accept socket success

          //fdindexa is right, then add to array and set flag
          fd_array[fdindexa] = connfd;
          fd_flag[fdindexa] = 1;
          if (fdm <fdindexa) //if need, update max fd flag: fdm
              fdm = fdindexa;
          if (maxfd < connfd)//if need, update max fd: maxfd
              maxfd = connfd;  
          
          printf("maxfd is %d, fdm is %d, fdindexa is %d\n", maxfd, fdm, fdindexa);
          
          FD_SET(connfd, &readfds);
          FD_SET(connfd, &writefds);
          FD_SET(connfd, &exceptfds);
          // need confirm *****
          //continue;
        }
        
        for (i=1; i<fdm+1; i++){
            if (fd_flag[i] == 0)
                continue;
            //flag is true
            if (( fd_flag[i] == 1)
                  && FD_ISSET(fd_array[i], &readfds))
            {
                n = recv(fd_array[i], buff, MAXLINE, 0);
                if (n <= 0){
                  fd_flag[i] = 0;
                  continue;
                }
                buff[n] = '\0';
                printf("recv msg from client %d: %s\n", fd_array[i], buff);
                           
                if (send(fd_array[i], buff, strlen(buff), 0) < 0) {
                  printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
                  continue;
                }    
                //fd_flag[i] = 0;
                //close(fd_array[i]);
            }
        }

          
          
    }//end while (1)


    close(listenfd);
    return 0;
}

int checkAvailablefd(int fdarray[], int fdflag[], int fdm){
    //return mini available fd
    return fdm+1;
}
void setFd(int fdarray[], int fdflag[], int fdm, fd_set* readfds, fd_set* writefds, fd_set* exceptfds){
    int i=0;
    if (fdm > MAXCONN)
        fdm = MAXCONN; 
    
    for (i=0; i<fdm+1; i++){
      if (fdflag[i] == 0)
          continue;
      //flag true
      if (readfds!= NULL)
          FD_SET(fdarray[i], readfds);
      if (writefds!= NULL)
          FD_SET(fdarray[i], writefds);
      if (exceptfds!= NULL)
          FD_SET(fdarray[i], exceptfds);
    }
    return;
}


int socket_client() {
    int sockfd, n;
    char recvline[4096], sendline[4096];
    struct sockaddr_in servaddr;
    char ipaddr[30];//--
    int i;//--
    char ch;


    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    for (i=0; '\n' != (ch=getchar()); i++)
        ipaddr[i]=ch;
    
    ipaddr[i] = '\0';
    memset(&servaddr, 0, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12345);
    if (inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) <= 0) {
        printf("inet_pton error for %s\n", ipaddr);
        //exit(0);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof (servaddr)) < 0) {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        //exit(0);
        return -1;
    }

    printf("send msg to server: \n");
    fgets(sendline, 4096, stdin);
    if (send(sockfd, sendline, strlen(sendline), 0) < 0) {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        //exit(0);
        return -1;
    }

    close(sockfd);
    return 0;
    //exit(0);
}