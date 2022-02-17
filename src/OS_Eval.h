#ifndef __OS_EVAL_H
#define __OS_EVAL_H

typedef struct testInfo {
	int iter;
	const char *name;
} testInfo;

void forkTest(struct timespec *childTime, struct timespec *parentTime) 
void threadTest(struct timespec *childTime, struct timespec *parentTime)

#endif

