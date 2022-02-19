#include "OS_Eval.h"

void recv_test(struct timespec *timeArray, int iter, int *i) {
	int retval;
	int fds1[2], fds2[2];
	retval = pipe(fds1);
	if (retval != 0) {
		printf("[error] failed to open pipe1.\n");
	}
	retval = pipe(fds2);
	if (retval != 0) {
		printf("[error] failed to open pipe2.\n");
	}
	char w = 'b', r;

	struct sockaddr_un server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_un));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, home, sizeof(server_addr.sun_path) - 1);
	strncat(server_addr.sun_path, sock, sizeof(server_addr.sun_path) - 1);

	int forkId = fork();

	if (forkId < 0) {
		printf("[error] fork failed.\n");
		return;
	}

	if (forkId > 0) {
		close(fds1[0]);
		close(fds2[1]);

		int fd_server = socket(PF_UNIX, SOCK_STREAM, 0);
		if (fd_server < 0) printf("[error] failed to open server socket.\n");

		retval = bind(fd_server, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un));
		if (retval == -1) printf("[error] failed to bind.\n");
		retval = listen(fd_server, 10);
		if (retval == -1) printf("[error] failed to listen.\n");
		if (DEBUG) printf("Waiting for connection\n");

		write(fds1[1], &w, 1);

		int fd_connect = accept(fd_server, (struct sockaddr *)0,
					(socklen_t *)0);
		if (DEBUG) printf("Connection accepted.\n");

		read(fds2[0], &r, 1);


		char *buf = (char *) malloc (sizeof(char) * msg_size);

		struct timespec startTime, endTime;
		retval = recv(fd_connect, buf, msg_size, MSG_DONTWAIT);
		for (int j = 0; *i < iter & j < curr_iter_limit; (*i) ++, j++) {

			clock_gettime(CLOCK_MONOTONIC,&startTime);
			retval = syscall(SYS_recvfrom, fd_connect, buf, msg_size, MSG_DONTWAIT, NULL, NULL);
			clock_gettime(CLOCK_MONOTONIC,&endTime);

			add_diff_to_sum(&timeArray[*i], endTime, startTime);

			if (retval == -1) {
				printf("[error] failed to recv.\n");
			}
		}

		write(fds1[1], &w, 1);

		remove(server_addr.sun_path);
		close(fd_server);
		close(fd_connect);
		close(fds1[1]);
		close(fds2[0]);
		free(buf);
		int status;
        	wait(&status);

	} else {
		close(fds1[1]);
		close(fds2[0]);

		read(fds1[0], &r, 1);

		int fd_client = socket(PF_UNIX, SOCK_STREAM, 0);
		if (fd_client < 0) printf("[error] failed to open client socket.\n");
		retval = connect(fd_client, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un));
		if (retval == -1)printf("[error] failed to connect.\n");

		int sockoption = 100000;
   		if(setsockopt(fd_client, SOL_SOCKET, SO_SNDBUF, &sockoption, sizeof(sockoption)) < 0) {
       			printf("[error] failed to set send buffer size to %d\n", sockoption);
   		}

		char *buf = (char *) malloc (sizeof(char) * msg_size);
		for (int i = 0; i < msg_size; i++) {
			buf[i] = 'a';
		}

		for (int j = 0; j < curr_iter_limit + 1; (*i) ++, j++) {

			retval = syscall(SYS_sendto, fd_client, buf, msg_size, MSG_DONTWAIT, NULL, 0);

			if (retval == -1) {
				printf("[error] failed to send.\n");
			}
		}

		write(fds2[1], &w, 1);
		read(fds1[0], &r, 1);
		close(fd_client);
		close(fds1[0]);
		close(fds2[1]);
		free(buf);

        	kill(getpid(),SIGINT);
		printf("[error] unable to kill child process\n");
		return;

	}

}

int main(int argc, char *argv[]){
	testInfo info;

        if (argc != 3){printf("Invalid arguments, gave %d not 3.\n",argc);return(0);}

        char *test_size = argv[1];
        int iteration = atoi(argv[2]);

        info.iter = iteration;


        if(strcmp(test_size, "base") == 0){
                msg_size = 1;
                curr_iter_limit = 50;

                info.name = "base recv";

                one_line_test_v2(recv_test, &info);
        }

        else if(strcmp(test_size, "big") == 0){
                msg_size = 96000;
                curr_iter_limit = 1;

                info.name = "big recv";

                one_line_test_v2(recv_test, &info);
        }


        else{printf("Invalid arguments, recv test type not valid.\n");return(0);}
}
