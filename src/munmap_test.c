#include "OS_Eval.h"

void munmap_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;

	int fd =open("test_file.txt", O_RDWR);
	if(fd < 0) printf("invalid fd%d\n", fd);
	void *addr = (void *)(intptr_t)__syscall(SYS_mmap, NULL, file_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
	for (int i = 0; i < file_size; i++) {
		((char *)addr)[i] = 'b';
	}
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	syscall(SYS_munmap, addr, file_size);
	clock_gettime(CLOCK_MONOTONIC,&endTime);
	close(fd);
	add_diff_to_sum(diffTime, endTime, startTime);
	return;
}
