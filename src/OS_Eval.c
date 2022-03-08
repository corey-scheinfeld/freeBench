#include "OS_Eval.h"

char *home = "/home/corey/freeBench/src/";
char *sock = "test_sockets/socket";

int file_size = -1;
int fd_count = -1;
bool is_local = false;
int msg_size = -1;
int curr_iter_limit = -1;

fileSpec file_tests[] = {
        {PAGE_SIZE, "small"},
        {PAGE_SIZE*10, "mid"},
        {PAGE_SIZE*1000, "big"},
        {PAGE_SIZE*10000, "huge"},
        {-1, ""}
};

fdSpec fd_tests[] = {
        {10, false, "base"},
        {1000, false, "big"},
	{10, true, "base_local"},
	{1000, true, "big_local"},
	{-1, false, ""}
};

msgSpec msg_tests[] = {
        {1, 50, "base"},
        {96000, 1,  "big"},
        {-1, -1, ""}
};



void add_diff_to_sum(struct timespec *result,struct timespec a, struct timespec b)
{
	if (result->tv_nsec +a.tv_nsec < b.tv_nsec)
	{
		result->tv_nsec = 1000000000+result->tv_nsec+a.tv_nsec-b.tv_nsec;
		result->tv_sec = result->tv_sec+a.tv_sec-b.tv_sec-1;
	}
	else if (result->tv_nsec+a.tv_nsec-b.tv_nsec >=1000000000)
	{
		result->tv_nsec = result->tv_nsec+a.tv_nsec-b.tv_nsec-1000000000;
		result->tv_sec = result->tv_sec+a.tv_sec-b.tv_sec+1;
	}
	else
	{
		result->tv_nsec = result->tv_nsec + a.tv_nsec - b.tv_nsec;
		result->tv_sec = result->tv_sec +a.tv_sec - b.tv_sec;
	}
}

struct timespec *calc_diff(struct timespec *smaller, struct timespec *bigger)
{
	struct timespec *diff = (struct timespec *)malloc(sizeof(struct timespec));
	if (smaller->tv_nsec > bigger->tv_nsec)
	{
		diff->tv_nsec = 1000000000 + bigger->tv_nsec - smaller->tv_nsec;
		diff->tv_sec = bigger->tv_sec - 1 - smaller->tv_sec;
	}
	else
	{
		diff->tv_nsec = bigger->tv_nsec - smaller->tv_nsec;
		diff->tv_sec = bigger->tv_sec - smaller->tv_sec;
	}
	return diff;
}

struct timespec* calc_average(struct timespec *sum, int size)
{
	struct timespec *average = (struct timespec *)malloc(sizeof(struct timespec));
	average->tv_sec = 0;
	average->tv_nsec = 0;
	if (size == 0) return average;

	average->tv_nsec = sum->tv_nsec / size + sum->tv_sec % size * 1000000000 / size;
	average->tv_sec = sum->tv_sec / size;
	return average;
}

void one_line_test(void (*f)(struct timespec*), testInfo *info){
	struct timespec testStart, testEnd;
	clock_gettime(CLOCK_MONOTONIC,&testStart);

	printf("%s benchmark experiment\nTest Name:, Iteration (x/%d):, Result (seconds)\n", info->name, info->iter);

	int runs = info->iter;

	struct timespec* timeArray = (struct timespec *)malloc(sizeof(struct timespec) * runs);
	for (int i=0; i < runs; i++) {
		timeArray[i].tv_sec = 0;
		timeArray[i].tv_nsec = 0;
		(*f)(&timeArray[i]);

		if(&timeArray[i] != NULL){printf("      %s time:, iter %d,  %ld.%09ld,\n", info->name, (i+1),timeArray[i].tv_sec, timeArray[i].tv_nsec);}
	}

	free(timeArray);

	clock_gettime(CLOCK_MONOTONIC,&testEnd);
	struct timespec *diffTime = calc_diff(&testStart, &testEnd);
	printf("      %s test total runtime:%ld.%09ld\n",info->name, diffTime->tv_sec, diffTime->tv_nsec); 
	free(diffTime);

	return;
}

void one_line_test_v2(void (*f)(struct timespec*, int, int *), testInfo *info){
	struct timespec testStart, testEnd;
	clock_gettime(CLOCK_MONOTONIC,&testStart);

	printf("%s benchmark experiment\nTest Name:, Iteration (x/%d):, Result (seconds)\n", info->name, info->iter);

	int runs = info->iter;

	struct timespec* timeArray = (struct timespec *)malloc(sizeof(struct timespec) * runs);

	for (int i = 0; i < runs; i++) {
		timeArray[i].tv_sec = 0;
		timeArray[i].tv_nsec = 0;
	}

	for (int i = 0; i < runs; ) {
		(*f)(timeArray, info->iter, &i);

	}

	for (int i = 0; i < runs; i++) {
		printf("      %s time:, iter %d,  %ld.%09ld,\n", info->name, (i+1),timeArray[i].tv_sec, timeArray[i].tv_nsec);
	}

	free(timeArray);

	clock_gettime(CLOCK_MONOTONIC,&testEnd);
	struct timespec *diffTime = calc_diff(&testStart, &testEnd);
	printf("      %s test total runtime:, , %ld.%09ld\n",info->name, diffTime->tv_sec, diffTime->tv_nsec); 
	free(diffTime);

	return;
}

void two_line_test(void (*f)(struct timespec*,struct timespec*), testInfo *info){
	struct timespec testStart, testEnd;
	clock_gettime(CLOCK_MONOTONIC,&testStart);
	printf("%s benchmark experiment\nTest Name:, Iteration (x/%d):, Result (seconds)\n", info->name, info->iter);

	int runs = info->iter;

	struct timespec* timeArrayParent = (struct timespec *) malloc(sizeof(struct timespec) * runs);
	struct timespec* timeArrayChild = (struct timespec *) malloc(sizeof(struct timespec) * runs);
	for (int i=0; i < runs; i++)
	{
		timeArrayParent[i].tv_sec = 0;
		timeArrayParent[i].tv_nsec = 0;
		timeArrayChild[i].tv_sec = 0;
		timeArrayChild[i].tv_nsec = 0;
		(*f)(&timeArrayChild[i],&timeArrayParent[i]);
		
		printf("      %s parent time:, iter %d,  %ld.%09ld,\n", info->name, (i+1), timeArrayParent[i].tv_sec, timeArrayParent[i].tv_nsec); 
		printf("      %s child time:, iter %d,  %ld.%09ld,\n", info->name, (i+1), timeArrayChild[i].tv_sec, timeArrayChild[i].tv_nsec); 

	}

	free(timeArrayChild);
	free(timeArrayParent);

	clock_gettime(CLOCK_MONOTONIC,&testEnd);
	struct timespec *diffTime = calc_diff(&testStart, &testEnd);
	printf("      %s test total runtime:, , %ld.%09ld\n",info->name, diffTime->tv_sec, diffTime->tv_nsec); 
	free(diffTime);
	return;
}

struct sockaddr_in *get_inet(int *servers, int *clients){
	struct sockaddr_in *server_adds = (struct sockaddr_in *)malloc(fd_count * sizeof(struct sockaddr_in));
	memset((void *)&server_adds[0], 0, (sizeof(struct sockaddr_in)*fd_count));

	int retval;
	int portNum = 50000;

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

		servers[i] = fd_server;
		clients[i] = fd_client;
		portNum++;
	}

	return server_adds;
}

struct sockaddr_un *get_unix(int *servers, int *clients){

	struct sockaddr_un *server_adds = (struct sockaddr_un *)malloc(fd_count * sizeof(struct sockaddr_un));
	memset((void *)&server_adds[0], 0, (sizeof(struct sockaddr_un)*fd_count));

	int retval;

	for (int i = 0; i < fd_count; i++) {

		char fd_num[4];
		sprintf(fd_num, "%d", i);

		server_adds[i].sun_family = AF_UNIX;
		strncat(server_adds[i].sun_path, home, sizeof(server_adds[i].sun_path) - 1);
		strncat(server_adds[i].sun_path, sock, sizeof(server_adds[i].sun_path) - 1);
		strncat(server_adds[i].sun_path, fd_num, sizeof(server_adds[i].sun_path) - 1);

		int fd_server = socket(PF_UNIX, SOCK_STREAM, 0);
		if (fd_server < 0) printf("invalid fd in local select: %d\n", fd_server);

		retval = bind(fd_server, (struct sockaddr *) &server_adds[i], sizeof(struct sockaddr_un));
		if (retval == -1) printf("[error] failed to bind.\n");

		retval = listen(fd_server, 10);
		if (retval == -1) printf("[error] failed to listen.\n");

		int fd_client = socket(PF_UNIX, SOCK_STREAM, 0);
		if (fd_client < 0) printf("[error] failed to open client socket.\n");

		retval = connect(fd_client, (struct sockaddr *) &server_adds[i], sizeof(struct sockaddr_un));
		if (retval == -1) printf("[error] failed to connect.\n");

		servers[i] = fd_server;
		clients[i] = fd_client;

	}	

	return server_adds;
}

