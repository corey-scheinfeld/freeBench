#include "OS_Eval.h"

void write_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;

	char *buf = (char *) malloc (sizeof(char) * file_size);
	for (int i = 0; i < file_size; i++) {
		buf[i] = 'a';
	}
	int fd = open("test_file.txt", O_CREAT | O_WRONLY);
	if (fd < 0) printf("invalid fd in write: %d\n", fd);

	clock_gettime(CLOCK_MONOTONIC, &startTime);
	syscall(SYS_write, fd, buf, file_size);
	clock_gettime(CLOCK_MONOTONIC,&endTime);

	close(fd);

	add_diff_to_sum(diffTime, endTime, startTime);
	free(buf);
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
        info.name = strcat(test_type, " write");

        one_line_test(write_test, &info);

        return(0);

}
