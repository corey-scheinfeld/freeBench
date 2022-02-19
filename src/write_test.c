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

        char *test_size = argv[1];
        int iteration = atoi(argv[2]);

        info.iter = iteration;


        if(strcmp(test_size, "small") == 0){
                file_size = PAGE_SIZE;

                info.name = "small write";

                one_line_test(write_test, &info);
        }

        else if(strcmp(test_size, "mid") == 0){
                file_size = PAGE_SIZE * 10;

                info.name = "mid write";

                one_line_test(write_test, &info);
        }

        else if(strcmp(test_size, "big") == 0){
                file_size = PAGE_SIZE * 1000;

                info.name = "big write";

                one_line_test(write_test, &info);
        }

        else if(strcmp(test_size, "huge") == 0){
                file_size = PAGE_SIZE * 10000;

                info.name = "huge write";

                one_line_test(write_test, &info);
        }

        else{printf("Invalid arguments, write test type not valid.\n");return(0);}
}
