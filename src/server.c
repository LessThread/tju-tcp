#include "tju_tcp.h"
#include <string.h>

int main(int argc, char **argv) {
    // 开启仿真环境 
    startSimulation();

    tju_tcp_t* my_server = tju_socket();
    
    tju_sock_addr bind_addr;

    tju_bind(my_server, bind_addr);

    tju_listen(my_server);


    char buf[2021];
    my_server->state = 0;
    tju_tcp_t* fd = tju_accept(my_server);
    printf("fd:%d\n",fd->state);
    
    while(1){
        if(tju_recv(fd, buf, fd->state)==-1){
            perror("recv");
		    exit(1);
        }
        if(tju_recv(fd, buf, fd->state)==0){
            break;
        }
        printf("server recv %s\n", buf);
    }
    return EXIT_SUCCESS;
}
