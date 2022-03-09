#include "OS_Eval.h"

void page_fault_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;

	int fd =open("test_file.txt", O_RDONLY);
	if (fd < 0) printf("invalid fd%d\n", fd);

	void *addr = (void *)(uintptr_t)__syscall(SYS_mmap, NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

	clock_gettime(CLOCK_MONOTONIC, &startTime);
	char a = *((char *)addr);
	clock_gettime(CLOCK_MONOTONIC,&endTime);

	syscall(SYS_munmap, addr, file_size);
        close(fd);
	add_diff_to_sum(diffTime, endTime, startTime);
	return;
}

int main(int argc, char *argv[]){
	testInfo info;

        if (argc != 3){printf("Invalid arguments, gave %d not 3.\n",argc);return(0);}

        char *test_type = argv[1];
        info.iter = atoi(argv[2]);

        fileSpec *option;
        for(option=&file_tests[0]; option->name; option++){
                if(strcmp(option->name, test_type) == 0){break;}
        }

        if (option->file_size == -1){fprintf(stderr, "Invalid test specification.\n");return(0);}

        file_size = option->file_size;
        info.name = strcat(test_type, " page_fault");

        one_line_test(page_fault_test, &info);

        return(0);

	
}
