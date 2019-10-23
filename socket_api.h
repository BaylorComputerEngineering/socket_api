//global prototypes
int ConfigureSendingSocket(int port);
int ConfigureReceivingSocket(char *my_ip_address, char *sender_ip_address, int sender_port_number);
int SendData(int *connfd, char* buffer);
int RecvData(int *socket_fd, char*buffer, char *my_ip_address, char *sender_ip_address, int sender_port_number);