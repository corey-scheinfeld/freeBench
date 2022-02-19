#include "OS_Eval.h"

void ref_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;

	clock_gettime(CLOCK_MONOTONIC, &startTime);
	clock_gettime(CLOCK_MONOTONIC,&endTime);

	add_diff_to_sum(diffTime, endTime, startTime);
	return;
}

int main(int argc, char *argv[]){
	testInfo info;

        if (argc != 2){printf("Invalid arguments, gave %d not 2.\n",argc);return(0);}

        int iteration = atoi(argv[1]);

        info.iter = iteration;
        info.name = "ref";
        one_line_test(ref_test, &info);


}
