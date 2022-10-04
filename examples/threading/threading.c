#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

//static pthread_mutex_t the_mutex = PTHREAD_MUTEX_INITIALIZER;


void ms_to_timespec(struct timespec* ts, int ms) {
    ts->tv_sec = ms / 1000;
	ts->tv_nsec = (ms % 1000) * 1000000;
}


void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    // struct thread_data* thread_func_args = (struct thread_data *) thread_param;

	thread_data_t* data = (thread_data_t *) thread_param;

	struct timespec ts_obtain, ts_release;

	ms_to_timespec(&ts_obtain, data->wait_to_obtain_ms);
	ms_to_timespec(&ts_release, data->wait_to_release_ms);

    nanosleep(&ts_obtain, NULL);

	pthread_mutex_lock(data->mutex);

	nanosleep(&ts_release, NULL);

	pthread_mutex_unlock(data->mutex);

	data->thread_complete_success = true;

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

	thread_data_t* data = (thread_data_t *) malloc(sizeof(thread_data_t));

	data->mutex = mutex;
	data->wait_to_obtain_ms = wait_to_obtain_ms;
	data->wait_to_release_ms = wait_to_release_ms;

	int rc = pthread_create(thread, NULL, threadfunc, (void *) data);

	if (rc != 0) {
		errno = rc;
		perror("pthread_create");
        return false;
	} else {
	    return true;
	}
}

