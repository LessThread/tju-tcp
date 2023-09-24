#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#ifndef _LOGGER_H_
#define _LOGGER_H_

pthread_mutex_t log_lock;

typedef enum
{
    LOG_SEND,
    LOG_RECV,
    LOG_CWND,
    LOG_RWND,
    LOG_SWND,
    LOG_RTTS,
    LOG_DELV,
    DEBUG_LOG
} LogEvent;

long getCurrentTime();
void initLogger();
void closeLogger();

void createLog(LogEvent event, ...);



#endif