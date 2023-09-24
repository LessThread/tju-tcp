
#include "rtt.h"

void insertTimeNode()
{
    TimeNode *q = (TimeNode *)malloc(sizeof(TimeNode));
    q->next = NULL;
    gettimeofday(&q->send, NULL);
    TimeNode *p = sendtimelist->first;
    if (p == NULL)
    {
        sendtimelist->first = q;
    }
    else
    {
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = q;
    }
    sendtimelist->length++;
}

void removeTopNode()
{
    if (sendtimelist->first != NULL)
    {
        TimeNode *p = sendtimelist->first->next;
        free(sendtimelist->first);
        sendtimelist->first = p;
        sendtimelist->length--;
    }
}

void cleanUackNode(int len)
{
    for (int i = len; i > 1; i--)
    {
        removeTopNode();
    }
}

void computeRtt(tju_tcp_t* sock,int range){
    while (pthread_mutex_lock(&sock->window.wnd_send->rtt_lock) != 0)
    {};

    if (sock->window.wnd_send->rttclk == 0) //重传引起
    {
        changeRttClk(sock,1);
        //sock->window.wnd_send->rttclk = 1;
        cleanUackNode(range);
        removeTopNode();
    }

    else   //响应正确
    {
        cleanUackNode(range);
        struct timeval nowtime;
        gettimeofday(&nowtime, NULL);
        uint32_t srtt = 1000000 * (nowtime.tv_sec - sendtimelist->first->send.tv_sec) +
                        (nowtime.tv_usec - sendtimelist->first->send.tv_usec);
        sock->window.wnd_send->ertt = 0.875 * sock->window.wnd_send->ertt + 0.125 * srtt;
        sock->window.wnd_send->drtt = 0.75 * sock->window.wnd_send->drtt + 0.25 * abs(sock->window.wnd_send->ertt - srtt);
        sock->window.wnd_send->RTO = sock->window.wnd_send->ertt + 4 * sock->window.wnd_send->drtt;

        createLog(LOG_RTTS,srtt/1000.0, sock->window.wnd_send->ertt/1000.0, sock->window.wnd_send->drtt/1000.0, sock->window.wnd_send->RTO/1000.0);
        //cleanUackNode(range);
        removeTopNode();
    }
    
    pthread_mutex_unlock(&sock->window.wnd_send->rtt_lock);
}