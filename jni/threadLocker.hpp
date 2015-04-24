/*
 * threadLocker.hpp
 *
 *  Created on: 2015/04/21
 *      Author: Administrator
 */

#ifndef THREADLOCKER_HPP_
#define THREADLOCKER_HPP_

#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct threadLock_{
	pthread_mutex_t m;
	pthread_cond_t  c;
	unsigned char   s;
} threadLock;

//----------------------------------------------------------------------
// thread Locks
// to ensure synchronisation between callbacks and processing code
class threadLocker
{
private:
	threadLock* p;
	struct timeval now;
	struct timespec timeout;

public:
	void createThreadLock()
	{
		p = (threadLock*) malloc(sizeof(threadLock));
		if (p == NULL)
		{
			return;
		}
		memset(p, 0, sizeof(threadLock));
		if (pthread_mutex_init(&(p->m), (pthread_mutexattr_t*) NULL) != 0) {
			free(p);
			return;
		}
		if (pthread_cond_init(&(p->c), (pthread_condattr_t*) NULL) != 0) {
			pthread_mutex_destroy(&(p->m));
			free(p);
			return;
		}
		p->s = (unsigned char) 1;

        LOGI("pthred mutex create");
	}

	void waitThreadLock()
    {
        pthread_mutex_lock(&(p->m));

        gettimeofday(&now, NULL);
    	timeout.tv_sec = now.tv_sec + 5;
    	timeout.tv_nsec = now.tv_usec * 1000;

    	while (!p->s) {
        	LOGI("pthred wait");
            pthread_cond_timedwait(&(p->c), &(p->m), &timeout);
        }
        p->s = (unsigned char) 0;
        pthread_mutex_unlock(&(p->m));
    }

    void notifyThreadLock()
    {
        pthread_mutex_lock(&(p->m));
        p->s = (unsigned char) 1;
        pthread_cond_signal(&(p->c));
        pthread_mutex_unlock(&(p->m));
    }

    void destroyThreadLock()
    {
        if (p == NULL)
            return;
        notifyThreadLock();
        pthread_cond_destroy(&(p->c));
        pthread_mutex_destroy(&(p->m));
        free(p);
        LOGI("pthred mutex destroy");
    }
};

#endif /* THREADLOCKER_HPP_ */
