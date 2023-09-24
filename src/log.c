// logger.c

#include "log.h"



static FILE *log_fp;

long getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void initLogger()
{
    pthread_mutex_init(&(log_lock), NULL);
    char hostname[8];
    gethostname(hostname, 8);
    if (strcmp(hostname, "server") == 0)
    {
        log_fp = fopen("/vagrant/tju_tcp/test/server.event.trace", "w+");
    }
    else if (strcmp(hostname, "client") == 0)
    {
        log_fp = fopen("./client.event.trace", "w+");
    }

    if (log_fp == NULL)
    {
        // 简单处理,实际可改为报错后退出
        log_fp = stderr;
    }
}

//使用可变参数统一处理日志
void createLog(LogEvent event, ...)
{
    while (pthread_mutex_lock(&log_lock) != 0){}

    fprintf(log_fp, "[%ld] ", getCurrentTime());

    if (event == LOG_SEND)
    {
        fprintf(log_fp, "[SEND] ");

        va_list args;
        va_start(args, event);

        unsigned seq = va_arg(args, unsigned);
        unsigned ack = va_arg(args, unsigned);
        unsigned flag = va_arg(args, unsigned);
        unsigned length = va_arg(args, unsigned);

        va_end(args);
        fprintf(log_fp, "[seq:%d ack:%d flag:%d length:%d]", seq, ack, flag, length);
    }
    else if (event == LOG_RECV)
    {
        fprintf(log_fp, "[RECV] ");

        va_list args;
        va_start(args, event);

        unsigned seq = va_arg(args, unsigned);
        unsigned ack = va_arg(args, unsigned);
        unsigned flag = va_arg(args, unsigned);
        unsigned length = va_arg(args, unsigned);

        va_end(args);
        fprintf(log_fp, "[seq:%d ack:%d flag:%d length:%d]", seq, ack, flag, length);
    }
    else if (event == LOG_CWND)
    {
        fprintf(log_fp, "[CWND] ");

        va_list args;
        va_start(args, event);

        unsigned type = va_arg(args, unsigned);
        unsigned size = va_arg(args, unsigned);

        va_end(args);
        fprintf(log_fp, "[type:%d size:%d]", type,size);
    }
    else if (event == LOG_RWND)
    {
        fprintf(log_fp, "[RWND] ");

        va_list args;
        va_start(args, event);

        unsigned size = va_arg(args, unsigned);

        va_end(args);
        fprintf(log_fp, "[size:%d]",size);
    }
    else if (event == LOG_SWND)
    {
        fprintf(log_fp, "[SWND] ");

        va_list args;
        va_start(args, event);

        unsigned size = va_arg(args, unsigned);

        va_end(args);
        fprintf(log_fp, "[size:%d]",size);
    }
    else if (event == LOG_DELV)
    {
        fprintf(log_fp, "[DELV] ");

        va_list args;
        va_start(args, event);

        unsigned type = va_arg(args, unsigned);
        unsigned size = va_arg(args, unsigned);

        va_end(args);
        fprintf(log_fp, "[seq:%d size:%d]", type,size);
    }
    else if (event == LOG_RTTS)
    {
        fprintf(log_fp, "[RTTS] ");

        va_list args;
        va_start(args, event);

        double SRTT = va_arg(args, double);
        double ERTT = va_arg(args, double);
        double DRTT = va_arg(args, double);
        double TI = va_arg(args, double);

        va_end(args);
        fprintf(log_fp, "SampleRTT:%f EstimatedRTT:%f DeviationRTT:%f TimeoutInterval:%f", SRTT,ERTT,DRTT,TI);
    }
    else if (event == DEBUG_LOG)
    {
        fprintf(log_fp,"[DEBUG] ");
    }
    
    

    fprintf(log_fp, "\n");
    fflush(log_fp);

    pthread_mutex_unlock(&log_lock);
}


void closeLogger()
{
    if (log_fp != stderr)
    {
        fclose(log_fp);
    }
}