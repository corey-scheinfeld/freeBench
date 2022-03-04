#include "OS_Eval.h"


void select_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO(&rfds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	int *servers = (int *)malloc(fd_count * sizeof(int)); 
	int *clients = (int *)malloc(fd_count * sizeof(int));
	
	int *maxFd;
	*maxFd = -1;
	int portNum = 50000;
	

	struct sockaddr_in *inet_adds;
	struct sockaddr_un *unix_adds;
	
	if(is_local){unix_adds = get_unix(servers, clients, maxFd);}
	else{inet_adds = get_inet(servers, clients, maxFd);}
	

	for (int i = 0; i < fd_count; i++) {
		FD_SET(servers[i], &rfds);
	}

	usleep(100);
	
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	retval = syscall(SYS_select, *maxFd + 1, &rfds, NULL, NULL, &tv);
	clock_gettime(CLOCK_MONOTONIC, &endTime);
	add_diff_to_sum(diffTime, endTime, startTime);

	if (retval != fd_count) {
		printf("[error] select return unexpected: %d.\n", retval);
	}

	for (int i = 0; i < fd_count; i++)
	{
		FD_CLR(servers[i], &rfds);
		if(is_local){remove(unix_adds[i].sun_path);}

		int retval = close(clients[i]);
		if (retval == -1) printf ("[error] client close failed in select test on socket %d.\n", clients[i]);

		retval = close(servers[i]);
		if (retval == -1) printf ("[error] server close failed in select test on socket %d.\n", servers[i]);

		}

	free(servers);
	free(clients);
	if(is_local){free(unix_adds);} else{free(inet_adds);}

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
	is_local = option->is_local;
        info.name = strcat(test_type, " select");

        one_line_test(select_test, &info);

        return(0);

}
