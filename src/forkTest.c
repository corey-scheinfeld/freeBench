#include "OS_Eval.h"

typedef struct testSpec {
        int page_count;
        const char *name;
} testSpec;

testSpec tests[] = {
	{0, "base"},
	{6000, "big"},
	{12000, "huge"},
	{-1, ""}
};

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
	
	if (argc != 3){fprintf(stderr, "Invalid arguments, gave %d not 3.\n",argc);return(0);}
	
	char *test_type = argv[1];
	info.iter = atoi(argv[2]);

	testSpec *option;
	for(option=&tests[0]; option->name; option++){
		if(strcmp(option->name, test_type) == 0){break;}
	}

	if (option->page_count == -1){fprintf(stderr, "Invalid test specification.\n");return(0);}

	page_count = option->page_count;
	info.name = strcat(test_type, "fork");

	void *pages[page_count];
	for (int i = 0; i < page_count; i++) {
    		pages[i] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	}

	two_line_test(forkTest, &info);

	for (int i = 0; i < page_count; i++) {
		munmap(pages[i], PAGE_SIZE);
	}

	return(0);
}

