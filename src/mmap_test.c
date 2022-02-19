#include "OS_Eval.h"

void mmap_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;

	int fd =open("test_file.txt", O_RDONLY);
	if (fd < 0) printf("invalid fd%d\n", fd);

	clock_gettime(CLOCK_MONOTONIC, &startTime);
	void *addr = (void *)(intptr_t)__syscall(SYS_mmap, NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
	clock_gettime(CLOCK_MONOTONIC,&endTime);
	
	syscall(SYS_munmap, addr, file_size);
        close(fd);
	add_diff_to_sum(diffTime, endTime, startTime);
	return;
}

int main(int argc, char *argv[]){
	testInfo info;

        if (argc != 3){printf("Invalid arguments, gave %d not 3.\n",argc);return(0);}

        char *test_size = argv[1];
        int iteration = atoi(argv[2]);

        info.iter = iteration;


        if(strcmp(test_size, "small") == 0){
                file_size = PAGE_SIZE;

                info.name = "small mmap";

                one_line_test(mmap_test, &info);
        }

        else if(strcmp(test_size, "mid") == 0){
                file_size = PAGE_SIZE * 10;

                info.name = "mid mmap";

                one_line_test(mmap_test, &info);
        }

        else if(strcmp(test_size, "big") == 0){
                file_size = PAGE_SIZE * 1000;

                info.name = "big mmap";

                one_line_test(mmap_test, &info);
        }

        else if(strcmp(test_size, "huge") == 0){
                file_size = PAGE_SIZE * 10000;

                info.name = "huge mmap";

                one_line_test(mmap_test, &info);
        }

        else{printf("Invalid arguments, mmap test type not valid.\n");return(0);}
}
