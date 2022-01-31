#include "OS_Eval.h"

void forkTest(struct timespec *childTime, struct timespec *parentTime) 
{
    struct timespec timeA;
    struct timespec timeC;
    struct timespec *timeX = mmap(NULL, sizeof(struct timespec), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int status;
    clock_gettime(CLOCK_MONOTONIC,&timeA);

    int forkId = fork();
    if (forkId == 0){
        clock_gettime(CLOCK_MONOTONIC, timeX);
        kill(getpid(),SIGINT);
	printf("[error] unable to kill child process\n");
	return;
    } else if (forkId > 0){
        clock_gettime(CLOCK_MONOTONIC,&timeC);
        wait(&status);
	add_diff_to_sum(childTime,*timeX,timeA);
	add_diff_to_sum(parentTime,timeC,timeA);
    } else {
    	printf("[error] fork failed.\n");
    }
    munmap(timeX, sizeof(struct timespec));
    return;
}

