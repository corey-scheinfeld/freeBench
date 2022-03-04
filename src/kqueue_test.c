#include "OS_Eval.h"

void kqueue_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;
	int retval;
	struct timespec tv;

        tv.tv_sec = 0;
        tv.tv_nsec = 0;


	int *servers = (int *)malloc(fd_count * sizeof(int)); 
	int *clients = (int *)malloc(fd_count * sizeof(int));
	
	struct sockaddr_in *inet_adds;	
	struct sockaddr_un *unix_adds;

	struct kevent events[fd_count];
	int portNum = 50000;

	int kqfd = kqueue();

	if(is_local){unix_adds = get_unix(servers, clients);}
	else{inet_adds = get_inet(servers, clients);}

	for (int i = 0; i < fd_count; i++) {
		EV_SET(&events[i], servers[i], EVFILT_READ, EV_ADD, 0, 0, 0);
		portNum++;
	}

	if(!is_local){usleep(150);}

	struct kevent *eventlist = (struct kevent *)malloc(fd_count * sizeof(struct kevent));
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	retval = kevent(kqfd, events, fd_count, eventlist, fd_count, &tv);
	clock_gettime(CLOCK_MONOTONIC, &endTime);
	add_diff_to_sum(diffTime, endTime, startTime);

	free(eventlist);
	if (retval != fd_count) {
		printf("[error] kqueue return unexpected: %d\n", retval);
	}

	retval = close(kqfd);
	if (retval == -1) printf ("[error] close kqfd failed in kqueue test %d.\n", kqfd);
	for (int i = 0; i < fd_count; i++) {

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
        info.name = strcat(test_type, " kqueue");

        one_line_test(kqueue_test, &info);

        return(0);

	
}
