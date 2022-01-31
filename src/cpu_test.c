#include "OS_Eval.h"

void cpu_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;

	double start = 9903290.789798798;
	double div = 3232.32;
	clock_gettime(CLOCK_MONOTONIC,&startTime);
	for (int i = 0; i < 500000; i ++) {
	  start = start / div;
	}
	clock_gettime(CLOCK_MONOTONIC,&endTime);

	add_diff_to_sum(diffTime, endTime, startTime);
	return;
}
