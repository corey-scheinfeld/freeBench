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
