//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h> 
#include "socket_api.h"

int send_socket_fd;
int ConfigureSendingSocket(int port)
{
	int portno, n, connfd;
	int ticks = 0;
    struct sockaddr_in server_addr;
    struct sockaddr_in localaddr;
    struct sockaddr_in clientaddr;

	printf("Creating Sending Socket\n");
    send_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    const int       optVal = 1;
    const socklen_t optLen = sizeof(optVal);
    int rtn = setsockopt(send_socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);
	if (send_socket_fd == -1)
	{
	   printf("Could not create socket, error = %s\n", strerror(errno));
	   return -1;
	}
	
    localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("192.168.6.182");
	localaddr.sin_port = htons( port );  // Any local port will do

	int bind_status = bind(send_socket_fd, (struct sockaddr *)&localaddr, sizeof(localaddr));
	if(bind_status < 0)
	{
	    printf("Bind failed, error = %s\n", strerror(errno));
	    return -1;
	}

	int listenfd = listen(send_socket_fd, 10);

    connfd = accept4(send_socket_fd, (struct sockaddr*)NULL, NULL, 0);
    if(connfd < 0)
    {
        printf("Connection accept failed, error = %s\n", strerror(errno));
	    return -1;
    }
    return connfd;
}

int SendData(int *connfd_in, char *buffer)
{
    int connfd = *connfd_in;
    int reconnect = 0;
    int status = 0;
    if(connfd > 0)
    {
        //printf("Before write\n");
        int ret = send(connfd, buffer, strlen(buffer), MSG_NOSIGNAL);
    	//write(connfd, buffer, strlen(buffer));
    	//printf("After write\n");
    	if(ret == -1)
    	{
    	    //printf("Broken Pipe!!!!!\n");
    	    reconnect = 1;
    	}
    }
    else
    {
        status = -1;
    	printf("Client disconnected\n");
    }
    
    if(reconnect == 1)
    {
        printf("Client disconnected, original connfd = %d\n", connfd);
        reconnect = 0;
    	
    	int listenfd = listen(send_socket_fd, 10);

        connfd = accept4(send_socket_fd, (struct sockaddr*)NULL, NULL, 0);
        if(connfd < 0)
        {
            printf("Connection accept failed, error = %s\n", strerror(errno));
	        return -1;
        }
        printf("Reconnected with connfd = %d\n", connfd);
        *connfd_in = connfd;
    }
    	
    return status;
}
		   
struct sockaddr_in client_addr;		   
		   
int ConfigureReceivingSocket(char *my_ip_address, char *sender_ip_address, int sender_port_number)
{
    int socket_fd;
    
    struct sockaddr_in localaddr;
	
    //Create socket
    socket_fd = socket(AF_INET , SOCK_STREAM, 0);
    const int       optVal = 1;
    const socklen_t optLen = sizeof(optVal);

    int rtn = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);

    if (socket_fd == -1)
    {
        printf("Could not create socket, error = %s\n", strerror(errno));
        return -1;
    }
    
    memset(&localaddr, '0', sizeof(localaddr)); 
    localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = inet_addr(my_ip_address);
	localaddr.sin_port = htons(5001);  // Any local port will do
	bind(socket_fd, (struct sockaddr *)&localaddr, sizeof(localaddr));
	
	
    memset(&client_addr, '0', sizeof(client_addr));  
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons( sender_port_number );
    if(inet_pton(AF_INET, sender_ip_address, &client_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured, error = %s\n", strerror(errno));
        return -1;
    } 
 
    //Connect to remote server
    printf("Attempting to connect to server\n");
    while (connect(socket_fd , (struct sockaddr *)&client_addr , sizeof(client_addr)) < 0)
    {
        sleep(5);
        printf("Retrying attempt to connect to server\n");
    }
     
    printf("Connected to server\n");
    return socket_fd;
}

int RecvData(int *socket_fd_in, char *buffer, char *my_ip_address, char *sender_ip_address, int sender_port_number)
{
    int socket_fd = *socket_fd_in;
	int value = 0;
	int status = 0;

    if( recv(socket_fd, buffer , 256 , 0) < 0)
    {
        printf("recv failed, error = %s\n", strerror(errno));
        status = -1;
    }
    value = atoi(buffer);
    if(value == 0)
    {
 /*       printf("Recv Socket Disconnected\n");
        
        //Connect to remote server
        printf("Attempting to connect to server\n");
        while (connect(socket_fd , (struct sockaddr *)&client_addr , sizeof(client_addr)) < 0)
        {
            sleep(5);
            printf("Retrying attempt to connect to server\n");
        }
        *socket_fd_in = socket_fd;
        printf("Connected to server\n");*/
        ConfigureReceivingSocket(my_ip_address, sender_ip_address, sender_port_number);
    }
    return status;
}