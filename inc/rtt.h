#include "global.h"
#include "tju_packet.h"
#include "tju_tcp.h"

#ifndef _RTT_H_
#define _RTT_H_

void insertTimeNode();
void removeTopNode();
void cleanUackNode(int len);
void computeRtt(tju_tcp_t* sock,int range);

typedef struct TimeNode 
{
	struct timeval send;
	struct TimeNode* next;
}TimeNode;

typedef struct{
    struct TimeNode* first;
	int length;
}timelinklist;

#endif