#include "OS_Eval.h"

void select_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO(&rfds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	int servers[fd_count];
	int clients[fd_count];
	int maxFd = -1;
	int portNum = 50000;

	struct sockaddr_in *server_adds = (struct sockaddr_in *)malloc(fd_count * sizeof(struct sockaddr_in));
	memset((void *)&server_adds[0], 0, (sizeof(struct sockaddr_in)*fd_count));

	for (int i = 0; i < fd_count; i++) {

		bzero(&server_adds[i], sizeof(server_adds[i]));
		server_adds[i].sin_family = AF_INET;
		server_adds[i].sin_port = htons(portNum);
		
		if(INADDR_ANY){ server_adds[i].sin_addr.s_addr = htonl(INADDR_ANY); }

		int fd_server = socket(PF_INET, SOCK_STREAM, 0);
		if (fd_server < 0) printf("invalid fd in select: %d\n", fd_server);

		int sockoption = 1;
		if(setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &sockoption, sizeof(sockoption)) < 0) {
       			printf("[error] failed to enable sock address reuse. \n");
   		}

		retval = bind(fd_server, (struct sockaddr *) &server_adds[i], sizeof(struct sockaddr_in));
		if (retval == -1) printf("[error] failed to bind.\n");

		retval = listen(fd_server, 10);
		if (retval == -1) printf("[error] failed to listen.\n");

		int fd_client = socket(PF_INET, SOCK_STREAM, 0);
		if (fd_client < 0) printf("[error] failed to open client socket.\n");

		retval = connect(fd_client, (struct sockaddr *) &server_adds[i], sizeof(struct sockaddr_in));
		if (retval == -1) printf("[error] failed to connect.\n");

		if (fd_server > maxFd) maxFd = fd_server;
		
		FD_SET(fd_server, &rfds);
		servers[i] = fd_server;
		clients[i] = fd_client;
		portNum++;
	}

	usleep(100);
	
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	retval = syscall(SYS_select, maxFd + 1, &rfds, NULL, NULL, &tv);
	clock_gettime(CLOCK_MONOTONIC, &endTime);
	add_diff_to_sum(diffTime, endTime, startTime);

	if (retval != fd_count) {
		printf("[error] select return unexpected: %d.\n", retval);
	}

	for (int i = 0; i < fd_count; i++)
	{
		FD_CLR(servers[i], &rfds);
		
		int retval = close(clients[i]);
		if (retval == -1) printf ("[error] client close failed in select test on socket %d.\n", clients[i]);

		retval = close(servers[i]);
		if (retval == -1) printf ("[error] server close failed in select test on socket %d.\n", servers[i]);

		}

	free(server_adds);
	return;
}

int main(int argc, char *argv[]){
	testInfo info;

        if (argc != 3){printf("Invalid arguments, gave %d not 3.\n",argc);return(0);}

        char *test_type = argv[1];
        info.iter = atoi(argv[2]);

        fdSpec *option;
        for(option=&fd_tests[0]; option->name; option++){
                if(strcmp(option->name, test_type) == 0){break;}
        }

        if (option->fd_count == -1){fprintf(stderr, "Invalid test specification.\n");return(0);}

        fd_count = option->fd_count;
        info.name = strcat(test_type, " select");

        one_line_test(select_test, &info);

        return(0);

	
}
