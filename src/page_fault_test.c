#include "OS_Eval.h"

void page_fault_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;

	int fd =open("test_file.txt", O_RDONLY);
	if (fd < 0) printf("invalid fd%d\n", fd);

	void *addr = (void *)(intptr_t)__syscall(SYS_mmap, NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

	clock_gettime(CLOCK_MONOTONIC, &startTime);
	char a = *((char *)addr);
	clock_gettime(CLOCK_MONOTONIC,&endTime);

	printf("read: %c\n", a);
	syscall(SYS_munmap, addr, file_size);
        close(fd);
	add_diff_to_sum(diffTime, endTime, startTime);
	return;
}

