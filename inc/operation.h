#include "global.h"
#include "tju_packet.h"
#include "tju_tcp.h"

#ifndef _OP_H_
#define _OP_H_

#define LISTEN_SOCKS 1
#define ESTABLISHED_SOCKS 2

void putData2Buf(tju_tcp_t *sock, char *pkt);
char *createSynPkt(tju_tcp_t *sock);
char *getExpectedpkt(tju_tcp_t *sock);
uint32_t getOutOrderBitSize(tju_tcp_t *sock);
int getOutOrderNum(tju_tcp_t* sock);
void generatePkt(tju_tcp_t*sock,const void * buffer,int len);
long getctime();
uint32_t min(uint32_t a,uint32_t b);

void setSocket2Kernel(tju_tcp_t* sock,int type);
void cleanSocketInKernel(tju_tcp_t* sock,int type);

void changeAckAnt(tju_tcp_t* sock,int num);
void changeRttClk(tju_tcp_t* sock,int num);
void setWinSendRwnd(tju_tcp_t* sock,int num);
void changeWinState(tju_tcp_t* sock,int state);
void setWinSendCwnd(tju_tcp_t* sock,int num);
void changeClk(tju_tcp_t* sock,int num);

#endif