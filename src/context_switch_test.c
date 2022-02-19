#include "OS_Eval.h"

void context_switch_test(struct timespec *diffTime) {
	int iter = 1000;
	struct timespec startTime, endTime;
	int fds1[2], fds2[2], retval;
	retval = pipe(fds1);
	if (retval != 0) printf("[error] failed to open pipe1.\n");
	retval = pipe(fds2);
	if (retval != 0) printf("[error] failed to open pipe2.\n");

	char w = 'a', r;
	cpuset_t cpuset;
	int prio;

	retval = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, getpid(), sizeof(cpuset), &cpuset);
	if (retval == -1) printf("[error] failed to get affinity.\n");
	prio = getpriority(PRIO_PROCESS, 0);
	if (prio == -1) printf("[error] failed to get priority.\n");

	int forkId = fork();
	if (forkId > 0) { // is parent
		retval = close(fds1[0]);
		if (retval != 0) printf("[error] failed to close fd1.\n");
		retval = close(fds2[1]);
		if (retval != 0) printf("[error] failed to close fd2.\n");

		cpuset_t set;
		CPU_ZERO(&set);
		CPU_SET(0, &set);
		retval = cpuset_setaffinity(CPU_LEVEL_WHICH,CPU_WHICH_PID, getpid(), sizeof(set), &set);
		if (retval == -1) printf("[error] failed to set processor affinity.\n");
		retval = setpriority(PRIO_PROCESS, 0, -20);
		if (retval == -1) printf("[error] failed to set process priority.\n");

		read(fds2[0], &r, 1);

		clock_gettime(CLOCK_MONOTONIC, &startTime);
		for (int i = 0; i < iter; i++) {
			write(fds1[1], &w, 1);
			read(fds2[0], &r, 1);
		}
		clock_gettime(CLOCK_MONOTONIC, &endTime);
		int status;
        	wait(&status);

		close(fds1[1]);
		close(fds2[0]);


	} else if (forkId == 0){

		retval = close(fds1[1]);
		if (retval != 0) printf("[error] failed to close fd1.\n");
		retval = close(fds2[0]);
		if (retval != 0) printf("[error] failed to close fd2.\n");

		cpuset_t set;
		CPU_ZERO(&set);
		CPU_SET(0, &set);
		retval = cpuset_setaffinity(CPU_LEVEL_WHICH,CPU_WHICH_PID, getpid(), sizeof(set), &set);
		if (retval == -1) printf("[error] failed to set processor affinity.\n");
		retval = setpriority(PRIO_PROCESS, 0, -20);
		if (retval == -1) printf("[error] failed to set process priority.\n");

		write(fds2[1], &w, 1);
		for (int i = 0; i < iter; i++) {
			read(fds1[0], &r, 1);
			write(fds2[1], &w, 1);
		}

        	kill(getpid(), SIGINT);
		printf("[error] unable to kill child process\n");
		return;
	} else {
		printf("[error] failed to fork.\n");
	}

	retval = cpuset_setaffinity(CPU_LEVEL_WHICH,CPU_WHICH_PID, getpid(), sizeof(cpuset), &cpuset);
	if (retval == -1) printf("[error] failed to restore affinity.\n");
	retval = setpriority(PRIO_PROCESS, 0, prio);
	if (retval == -1) printf("[error] failed to restore priority.\n");

	struct timespec sum;
	sum.tv_sec = 0;
	sum.tv_nsec = 0;
	add_diff_to_sum(&sum, endTime, startTime);
	struct timespec *diff = calc_average(&sum, iter);
	diffTime->tv_sec = diff->tv_sec;
	diffTime->tv_nsec = diff->tv_nsec;
	free(diff);
}

int main(int argc, char *argv[]){
	testInfo info;

        if (argc != 2){printf("Invalid arguments, gave %d not 2.\n",argc);return(0);}

        int iteration = atoi(argv[1]);

        info.iter = iteration;
        info.name = "context switch";
        one_line_test(context_switch_test, &info);


}
