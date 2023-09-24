
#include "operation.h"

//封装的改值函数（加锁）
void changeClk(tju_tcp_t* sock,int num)
{
    while (pthread_mutex_lock(&(sock->window.wnd_send->clk_lock)) != 0)
    {
    }
    
    sock->window.wnd_send->clk = num;

    pthread_mutex_unlock(&(sock->window.wnd_send->clk_lock));
}
void changeAckAnt(tju_tcp_t* sock,int num)
{
    while (pthread_mutex_lock(&(sock->window.wnd_send->ackcnt_lock)) != 0)
    {
    }
    
    sock->window.wnd_send->ack_cnt = num;

    pthread_mutex_unlock(&(sock->window.wnd_send->ackcnt_lock));
}
void changeRttClk(tju_tcp_t* sock,int num)
{
    while (pthread_mutex_lock(&(sock->window.wnd_send->rttclk_lock)) != 0)
    {
    }
    
    sock->window.wnd_send->rttclk = num;

    pthread_mutex_unlock(&(sock->window.wnd_send->rttclk_lock));
}
void setWinSendRwnd(tju_tcp_t* sock,int num)
{
    while (pthread_mutex_lock(&(sock->window.wnd_send->rwnd_lock)) != 0)
    {
    }
    
    sock->window.wnd_send->rwnd = num;

    pthread_mutex_unlock(&(sock->window.wnd_send->rwnd_lock));
}
void setWinSendCwnd(tju_tcp_t* sock,int num){
    while (pthread_mutex_lock(&(sock->window.wnd_send->cwnd_lock)) != 0)
    {
    }
    
    sock->window.wnd_send->cwnd = num;

    pthread_mutex_unlock(&(sock->window.wnd_send->cwnd_lock));
}
void setWinSendSsthresh(tju_tcp_t* sock,int num){
    while (pthread_mutex_lock(&(sock->window.wnd_send->ssthresh_lock)) != 0)
    {
    }
    
    sock->window.wnd_send->ssthresh = num;
    if (sock->window.wnd_send->ssthresh < MAX_DLEN)
    {
         sock->window.wnd_send->ssthresh = MAX_DLEN;
    }

    pthread_mutex_unlock(&(sock->window.wnd_send->ssthresh_lock));
}

//封装内核监听函数
void setSocket2Kernel(tju_tcp_t* sock,int type){
    if (type == LISTEN_SOCKS){
        listen_socks[cal_hash(sock->bind_addr.ip,sock->bind_addr.port, 0, 0)] = sock;
    }

    if (type == ESTABLISHED_SOCKS){
        established_socks[cal_hash(sock->established_local_addr.ip,sock->established_local_addr.port, sock->established_remote_addr.ip, sock->established_remote_addr.port)] = sock;
    }
    
}
void cleanSocketInKernel(tju_tcp_t* sock,int type){
    if (type == LISTEN_SOCKS){
        listen_socks[cal_hash(sock->bind_addr.ip,sock->bind_addr.port, 0, 0)] = NULL;
    }

    if (type == ESTABLISHED_SOCKS){
        established_socks[cal_hash(sock->established_local_addr.ip,sock->established_local_addr.port, sock->established_remote_addr.ip, sock->established_remote_addr.port)] = NULL;
    }
}

//将数据包内容放入缓冲区
void putData2Buf(tju_tcp_t *sock, char *pkt)
{
    uint32_t data_len = get_plen(pkt) - DEFAULT_HEADER_LEN;

    while (pthread_mutex_lock(&(sock->recv_lock)) != 0){}

    if (sock->received_buf == NULL)
    {
        sock->received_buf = malloc(data_len);
    }
    else
    {
        sock->received_buf = realloc(sock->received_buf, sock->received_len + data_len);
    }
    memcpy(sock->received_buf + sock->received_len, pkt + DEFAULT_HEADER_LEN, data_len);
    sock->received_len += data_len;

    pthread_mutex_unlock(&(sock->recv_lock));
    return;
}

//简单封装SYN的包函数
char *createSynPkt(tju_tcp_t *sock)
{
    char *msg;
    uint32_t seq = 0;
    uint8_t flag = 0xff & SYN_FLAG_MASK;
    msg = create_packet_buf(sock->established_local_addr.port, sock->established_remote_addr.port,
                            seq, 0, DEFAULT_HEADER_LEN, DEFAULT_HEADER_LEN, flag, 1, 0, NULL, 0);
    return msg;
}

//ACK确认包函数（含Rwnd）
char *getExpectedpkt(tju_tcp_t *sock)
{
    char *msg;
    uint32_t seq = sock->window.wnd_send->nextseq;
    uint32_t ack = sock->window.wnd_recv->expect_seq;
    uint8_t flag = 0xff & ACK_FLAG_MASK;
    uint32_t Rwnd = 65535 - getOutOrderBitSize(sock);
    createLog(LOG_RWND, Rwnd);
    msg = create_packet_buf(sock->established_local_addr.port, sock->established_remote_addr.port, seq, ack,
                            DEFAULT_HEADER_LEN, DEFAULT_HEADER_LEN, flag, Rwnd, 0, NULL, 0);
    createLog(LOG_SEND, seq, ack, flag, 0);
    return msg;
}

//获取乱序数据包的字节总数
uint32_t getOutOrderBitSize(tju_tcp_t *sock)
{
    uint32_t count = 0;
    int index = 0;
    while (index < MAX_LEN && sock->window.wnd_recv->outoforder[index] != NULL)
    {
        count += get_plen(sock->window.wnd_recv->outoforder[index]) - DEFAULT_HEADER_LEN;
        index++;
    }
    return count;
}

//获取乱序数据包个数
int getOutOrderNum(tju_tcp_t *sock)
{
    int index = 0;
    while (index < MAX_LEN && sock->window.wnd_recv->outoforder[index] != NULL)
    {
        index++;
    }
    return index;
}

//生成数据包封装接口，略去0窗口
void generatePkt(tju_tcp_t *sock, const void *buffer, int len)
{
    char *data = malloc(len);
    memcpy(data, buffer, len);

    char *msg;
    uint32_t seq = sock->window.wnd_send->nextseq;
    uint16_t plen = DEFAULT_HEADER_LEN + len;

    //零窗口探测实现
    //char* ZeroMsg = create_packet_buf(sock->established_local_addr.port, sock->established_remote_addr.port, seq-1, sock->window.wnd_send->nextseq + len,
    //                        DEFAULT_HEADER_LEN,DEFAULT_HEADER_LEN, ACK_FLAG_MASK, 0, 0, NULL, 0);
    while ((sock->window.wnd_send->nextseq - sock->window.wnd_send->base + len) > sock->window.wnd_send->rwnd)
    {
        // if(sock->window.wnd_send->rwnd == 0)
        // {
        //     if(getCurrentTime()%1000000 == 99998)
        //         sendToLayer3(ZeroMsg,DEFAULT_HEADER_LEN);
        // }
    }
    //free(ZeroMsg);

    msg = create_packet_buf(sock->established_local_addr.port, sock->established_remote_addr.port, seq, sock->window.wnd_send->nextseq + len,
                            DEFAULT_HEADER_LEN, plen, ACK_FLAG_MASK, 0, 0, data, len);

    while (pthread_mutex_lock(&sock->send_lock) != 0)
    {
    };

    memcpy(sock->sending_buf + sock->window.wnd_send->lastmsg, msg, plen);
    sock->window.wnd_send->lastmsg += plen;
    sock->sending_len += plen;

    pthread_mutex_unlock(&(sock->send_lock));
    sock->window.wnd_send->nextseq += len;

    // 判断报文是否为序号最小的未被确认报文段
    if (sock->window.wnd_send->nextseq - sock->window.wnd_send->base == len)
    {
        while (pthread_mutex_lock(&sock->window.wnd_send->retran_lock) != 0)
        {
        }

        //开启计时器
        changeClk(sock,TRUE);

        gettimeofday(&(sock->window.wnd_send->send_time), NULL);
        sock->window.wnd_send->retran[0] = malloc(len + DEFAULT_HEADER_LEN);
        memcpy(sock->window.wnd_send->retran[0], msg, plen);

        //清零累积重复ACK计数器？
        //changeAckAnt(sock,0);


        pthread_mutex_unlock(&(sock->window.wnd_send->retran_lock));
    }
    else{
        createLog(DEBUG_LOG);
    }

    //加入RTT时间计算队列
    while (pthread_mutex_lock(&sock->window.wnd_send->rtt_lock) != 0){}

    insertTimeNode();

    pthread_mutex_unlock(&sock->window.wnd_send->rtt_lock);

    sendToLayer3(msg, plen);

    createLog(LOG_SEND, get_seq(msg), sock->window.wnd_send->nextseq, ACK_FLAG_MASK, len);
    return;
}

uint32_t min(uint32_t a, uint32_t b)
{
    return (a < b) ? a : b;
}

long getctime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void changeWinState(tju_tcp_t* sock,int state)
{
    while (pthread_mutex_lock(&(sock->window.wnd_send->state_lock)) != 0)
    {
    }
    
    sock->window.wnd_send->congestion_status = state;

    pthread_mutex_unlock(&(sock->window.wnd_send->state_lock));
}