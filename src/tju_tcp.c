#include "tju_tcp.h"
#include <sys/socket.h>

tju_tcp_t* tju_socket(){
    tju_tcp_t* sock = (tju_tcp_t*)malloc(sizeof(tju_tcp_t));
    sock->TCPsocket = socket(AF_INET, SOCK_STREAM, 0);
    return sock;
}


int tju_bind(tju_tcp_t* sock, tju_sock_addr bind_addr){
    return 0;
}


int tju_listen(tju_tcp_t* listen_sock){
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family 		= AF_INET;//IPv4
    server_sockaddr.sin_port 		= htons(1234);//端口
    server_sockaddr.sin_addr.s_addr 	= inet_addr("172.17.0.3");//本主机的任意IP都可以使用
    bzero(&(server_sockaddr.sin_zero),8);//保留的8字节置零

    //2.绑定 fd与 端口和地址
    bind(listen_sock->TCPsocket,(struct sockaddr *)&server_sockaddr,sizeof(struct sockaddr));
    listen(listen_sock->TCPsocket,10);
    return 0;
}


tju_tcp_t* tju_accept(tju_tcp_t* listen_sock){
    int sin_size;
    int fd = accept(listen_sock->TCPsocket,NULL,&sin_size);
    listen_sock->state = fd;
    return listen_sock;
}



int tju_connect(tju_tcp_t* sock, tju_sock_addr target_addr){
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("172.17.0.3");
	server.sin_family = AF_INET;
	server.sin_port = htons(1234);

    connect(sock->TCPsocket, (struct sockaddr *)&server, sizeof(server));
    printf("connect successful! \n");
    return 0;
}

int tju_send(tju_tcp_t* sock, const void *buffer, int len){
    send(sock->TCPsocket,buffer,len,0);
    return 0;
}
int tju_recv(tju_tcp_t* sock, void *buffer, int len){
    int bits = recv(sock->state, buffer, 12, 0);
    return bits;
}

int tju_handle_packet(tju_tcp_t* sock, char* pkt){
    return 0;
}

int tju_close (tju_tcp_t* sock){
    close(sock->TCPsocket);
    return 0;
}