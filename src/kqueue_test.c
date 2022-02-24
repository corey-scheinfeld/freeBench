#include "OS_Eval.h"

void kqueue_test(struct timespec *diffTime) {
	struct timespec startTime, endTime;
	int retval;
	struct timespec tv;

        tv.tv_sec = 0;
        tv.tv_nsec = 0;

	int fds[fd_count];
	int clients[fd_count];
	struct kevent events[fd_count];
	int sockNum = 50000;

	struct sockaddr_in *server_adds = (struct sockaddr_in *)malloc(fd_count * sizeof(struct sockaddr_in));
        memset((void *)&server_adds[0], 0, (sizeof(struct sockaddr_in)*fd_count));

	int kqfd = kqueue();

	for (int i = 0; i < fd_count; i++) {
		char fd_num[4];
                sprintf(fd_num, "%d", i);

		server_adds[i].sin_family = AF_INET;
		server_adds[i].sin_port = htons(sockNum);
		
		if(INADDR_ANY){ server_adds[i].sin_addr.s_addr = htonl(INADDR_ANY); }

             	char name[10];
		name[0] = 'f';
		name[1] = 'i';
		name[2] = 'l';
		name[3] = 'e';
		int j = i;
		int index = 4;
		while (j > 0) {
			name[index] =  48 + j%10;
			j = j/10;
			index ++;
		}
		name[index] = '\0';
		int fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd < 0) printf("[error] invalid fd in kqueue: %d\n", fd);

		EV_SET(&events[i], fd, EVFILT_READ, EV_ADD, 0, 0, 0);
		fds[i] = fd;

		int sockoption = 1;
		if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sockoption, sizeof(sockoption)) < 0) {
       			printf("[error] failed to enable sock address reuse. %d\n");
   		}

		retval = bind(fd, (struct sockaddr *) &server_adds[i], sizeof(struct sockaddr_in));
                if (retval == -1) printf("[error] failed to bind.\n");

                retval = listen(fd, 10);
                if (retval == -1) printf("[error] failed to listen.\n");

                int fd_client = socket(PF_INET, SOCK_STREAM, 0);
                if (fd_client < 0) printf("[error] failed to open client socket.\n");

                retval = connect(fd_client, (struct sockaddr *) &server_adds[i], sizeof(struct sockaddr_in));
                if (retval == -1) printf("[error] failed to connect.\n");

		clients[i] = fd_client;
		sockNum++;
	}

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
		retval = close(fds[i]);
		if (retval == -1) printf ("[error] close failed in kqueue test %d.\n", fds[i]);

		retval = close(clients[i]);
		if (retval == -1) printf ("[error] close failed in kqueue test %d.\n", clients[i]);
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
        info.name = strcat(test_type, " kqueue");

        one_line_test(kqueue_test, &info);

        return(0);

	
}
