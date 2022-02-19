#include "OS_Eval.h"

void select_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO(&rfds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	int fds[fd_count];
	int clients[fd_count];
	int maxFd = -1;

	struct sockaddr_un *server_adds = (struct sockaddr_un *)malloc(fd_count * sizeof(struct sockaddr_un));
	memset((void *)&server_adds[0], 0, (sizeof(struct sockaddr_un)*fd_count));

	for (int i = 0; i < fd_count; i++) {

		char fd_num[4];
		sprintf(fd_num, "%d", i);

		server_adds[i].sun_family = AF_UNIX;
		strncat(server_adds[i].sun_path, home, sizeof(server_adds[i].sun_path) - 1);
		strncat(server_adds[i].sun_path, sock, sizeof(server_adds[i].sun_path) - 1);
		strncat(server_adds[i].sun_path, fd_num, sizeof(server_adds[i].sun_path) - 1);

		int fd = socket(PF_UNIX, SOCK_STREAM, 0);
		if (fd < 0) printf("invalid fd in select: %d\n", fd);

		retval = bind(fd, (struct sockaddr *) &server_adds[i], sizeof(struct sockaddr_un));
		if (retval == -1) printf("[error] failed to bind.\n");

		retval = listen(fd, 10);
		if (retval == -1) printf("[error] failed to listen.\n");

		int fd_client = socket(PF_UNIX, SOCK_STREAM, 0);
		if (fd_client < 0) printf("[error] failed to open client socket.\n");

		retval = connect(fd_client, (struct sockaddr *) &server_adds[i], sizeof(struct sockaddr_un));
		if (retval == -1) printf("[error] failed to connect.\n");

		if (fd > maxFd) maxFd = fd;

		FD_SET(fd, &rfds);
		fds[i] = fd;
		clients[i] = fd_client;
	}

	clock_gettime(CLOCK_MONOTONIC, &startTime);
	retval = syscall(SYS_select, maxFd + 1, &rfds, NULL, NULL, &tv);
	clock_gettime(CLOCK_MONOTONIC, &endTime);
	add_diff_to_sum(diffTime, endTime, startTime);

	if (retval != fd_count) {
		printf("[error] select return unexpected: %d\n", retval);
	}

	for (int i = 0; i < fd_count; i++)
	{
		FD_CLR(fds[i], &rfds);
		remove(server_adds[i].sun_path);

		int retval = close(fds[i]);
		if (retval == -1) printf ("[error] close failed in select test %d.\n", fds[i]);

		retval = close(clients[i]);
		if (retval == -1) printf ("[error] close failed in select test %d.\n", clients[i]);

	}

	free(server_adds);
	return;
}

int main(int argc, char *argv[]){
	testInfo info;

        if (argc != 3){printf("Invalid arguments, gave %d not 3.\n",argc);return(0);}

        char *test_size = argv[1];
        int iteration = atoi(argv[2]);

        info.iter = iteration;


        if(strcmp(test_size, "small") == 0){
		fd_count = 10;

                info.name = "small select";

                one_line_test(select_test, &info);
        }

        else if(strcmp(test_size, "big") == 0){
                fd_count = 1000;

                info.name = "big select";

                one_line_test(select_test, &info);
        }

       
        else{printf("Invalid arguments, select test type not valid.\n");return(0);}
}
