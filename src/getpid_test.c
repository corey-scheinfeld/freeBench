#include "OS_Eval.h"

void getpid_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	syscall(SYS_getpid);
	clock_gettime(CLOCK_MONOTONIC, &endTime);
	add_diff_to_sum(diffTime, endTime, startTime);
	return;

}

int main(int argc, char *argv[]){

	testInfo info;

	if (argc != 2){printf("Invalid arguments, gave %d not 2.\n",argc);return(0);}

	info.iter = atoi(argv[1]);
	info.name = "getpid";
	one_line_test(getpid_test, &info);
}
