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

int main(int argc, char *argv[])
{
	testInfo info;
	int page_count;
	
	if (argc != 3){printf("Invalid arguments, gave %d not 3.\n",argc);return(0);}
	
	char *test_size = argv[1];
	int iteration = atoi(argv[2]);

	info.iter = iteration;

	if(strcmp(test_size, "base") == 0){
		info.name = "fork";

		two_line_test(forkTest, &info);
	}

	else if(strcmp(test_size, "big") == 0){
		info.name = "big fork";

		page_count = 6000;
		void *pages[page_count];
		for (int i = 0; i < page_count; i++) {
    			pages[i] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		}

		two_line_test(forkTest, &info);

		for (int i = 0; i < page_count; i++) {
			munmap(pages[i], PAGE_SIZE);
		}

	}

	else if(strcmp(test_size, "huge") == 0){
		info.name = "huge fork";

		page_count = 12000;
		printf("Page count: %d.\n", page_count);
		void *pages1[page_count];
		for (int i = 0; i < page_count; i++) {
    			pages1[i] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		}

		two_line_test(forkTest, &info);

		for (int i = 0; i < page_count; i++) {
			munmap(pages1[i], PAGE_SIZE);
		}
	}

	else{printf("Invalid arguments, fork test type not valid.\n");return(0);}


}

