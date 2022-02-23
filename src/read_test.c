#include "OS_Eval.h"

void read_test(struct timespec *diffTime) { 
	struct timespec startTime, endTime;
	char *buf_in = (char *) malloc (sizeof(char) * file_size);

	int fd =open("test_file.txt", O_RDONLY);
	if (fd < 0) printf("invalid fd in read: %d\n", fd);
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	syscall(SYS_read, fd, buf_in, file_size);
	clock_gettime(CLOCK_MONOTONIC, &endTime);
	close(fd);
	
	add_diff_to_sum(diffTime, endTime, startTime);
	free(buf_in);
	return;

}

void read_warmup() {

	char *buf_out = (char *) malloc (sizeof(char) * file_size);
	for (int i = 0; i < file_size; i++) {
		buf_out[i] = 'a';
	}

	int fd = open("test_file.txt", O_CREAT | O_WRONLY);
	if (fd < 0) printf("invalid fd in write: %d\n", fd);

	syscall(SYS_write, fd, buf_out, file_size);
	close(fd);	

	char *buf_in = (char *) malloc (sizeof(char) * file_size);

	fd =open("test_file.txt", O_RDONLY);
	if (fd < 0) printf("invalid fd in read: %d\n", fd);
	
	for (int i = 0; i < 1000; i ++) {
		syscall(SYS_read, fd, buf_in, file_size);
	}
	close(fd);

	free(buf_out);
	free(buf_in);
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
        info.name = strcat(test_type, " read");

	if (!(strcmp(info.name, "huge read") == 0)){read_warmup();}
        one_line_test(read_test, &info);

        return(0);

}
