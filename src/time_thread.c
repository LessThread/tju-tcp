
#include "time_thread.h"

void *startTimer(void *arg)
{
    tju_tcp_t *sock = (tju_tcp_t *)arg;
    struct timeval nowtime;
    while (1)
    {
        if (sock->window.wnd_send->clk == FALSE)
        {
        }
        else
        {
            gettimeofday(&nowtime, NULL);
            long Time = 1000000L * (nowtime.tv_sec - sock->window.wnd_send->send_time.tv_sec) +
                        (nowtime.tv_usec - sock->window.wnd_send->send_time.tv_usec);
            if (Time > sock->window.wnd_send->RTO)
            {
                if (sock->window.wnd_send->congestion_status == SLOW_START)
                {
                    setWinSendSsthresh( sock,sock->window.wnd_send->cwnd/2);
                    setWinSendCwnd(sock,MAX_DLEN);
                    createLog(LOG_CWND, 3, sock->window.wnd_send->cwnd);
                }
                else if (sock->window.wnd_send->congestion_status == CONGESTION_AVOIDANCE)
                {
                    setWinSendSsthresh( sock,sock->window.wnd_send->cwnd/2);
                    setWinSendCwnd(sock,MAX_DLEN);
                    createLog(LOG_CWND, 3, sock->window.wnd_send->cwnd);
                    changeWinState(sock,SLOW_START);
                }
                else
                {
                    setWinSendSsthresh( sock,sock->window.wnd_send->cwnd/2);
                    setWinSendCwnd(sock,MAX_DLEN);
                    createLog(LOG_CWND, 3, sock->window.wnd_send->cwnd);
                    changeWinState(sock,SLOW_START);
                }
                createLog(LOG_RWND, sock->window.wnd_send->rwnd);///

                setWinSendRwnd(sock,min(sock->window.wnd_send->cwnd, sock->window.wnd_send->rwnd));

                createLog(LOG_SWND, sock->window.wnd_send->rwnd);

                changeRttClk(sock,0);
                //sock->window.wnd_send->rttclk = 0;

                while (pthread_mutex_lock(&sock->window.wnd_send->retran_lock) != 0)
                {
                }

                sendToLayer3(sock->window.wnd_send->retran[0], get_plen(sock->window.wnd_send->retran[0]));
                createLog(LOG_SEND, get_seq(sock->window.wnd_send->retran[0]), get_ack(sock->window.wnd_send->retran[0]), ACK_FLAG_MASK, get_plen(sock->window.wnd_send->retran[0]) - DEFAULT_HEADER_LEN);

                gettimeofday(&sock->window.wnd_send->send_time, NULL);
                
                changeAckAnt(sock,0);
                //sock->window.wnd_send->ack_cnt = 0;

                pthread_mutex_unlock(&(sock->window.wnd_send->retran_lock));
            }
        }
    }
}