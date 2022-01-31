#ifndef __OS_EVAL_H
#define __OS_EVAL_H

#define __BSD_VISIBLE 1
#define FD_SETSIZE 2000
#include <sys/cdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/cpuset.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/event.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/errno.h>
#include <sys/select.h>
#include <math.h>


typedef struct testInfo {
	int iter;
	const char *name;
} testInfo;

extern int file_size;

void forkTest(struct timespec *childTime, struct timespec *parentTime); 
void threadTest(struct timespec *childTime, struct timespec *parentTime);
void add_diff_to_sum(struct timespec *result,struct timespec a, struct timespec b);
void getpid_test(struct timespec *diffTime);
void read_test(struct timespec *diffTime);
void read_warmup();
void write_test(struct timespec *diffTime);
void mmap_test(struct timespec *diffTime);
void page_fault_test(struct timespec *diffTime);
void cpu_test(struct timespec *diffTime);
void ref_test(struct timespec *diffTime);
void munmap_test(struct timespec *diffTime);
void select_test(struct timespec *diffTime);
void poll_test(struct timespec *diffTime);
void kqueue_test(struct timespec *diffTime);
void context_switch_test(struct timespec *diffTime);
void send_test(struct timespec *timeArray, int iter, int *i);
void recv_test(struct timespec *timeArray, int iter, int *i);

#endif

