#include "OS_Eval.h"

void ref_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;

	clock_gettime(CLOCK_MONOTONIC, &startTime);
	clock_gettime(CLOCK_MONOTONIC,&endTime);

	add_diff_to_sum(diffTime, endTime, startTime);
	return;
}
