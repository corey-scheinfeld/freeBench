#include "OS_Eval.h"

struct timespec *timeB;
struct timespec *timeD;

void *thrdfnc(void *args)
{
	clock_gettime(CLOCK_MONOTONIC,timeB);
	pthread_exit(NULL);
}

void threadTest(struct timespec *childTime, struct timespec *parentTime)
{
	struct timespec timeC;
	timeB=(struct timespec *)malloc(sizeof(struct timespec));
	timeD=(struct timespec *)malloc(sizeof(struct timespec));
	pthread_t newThrd;
	clock_gettime(CLOCK_MONOTONIC,timeD);
	int er = pthread_create (&newThrd, NULL, thrdfnc, NULL);
	clock_gettime(CLOCK_MONOTONIC,&timeC);
	pthread_join(newThrd,NULL);

	add_diff_to_sum(parentTime,timeC,*timeD);
	add_diff_to_sum(childTime,*timeB,*timeD);
	
	free(timeB);
	timeB = NULL;
	free(timeD);
	timeD = NULL;
	return;
}
