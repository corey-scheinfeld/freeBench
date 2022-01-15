#include <sys/select.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
        int fd_count = 1;
	fd_set rfds;
        struct timeval tv;
        int retval;

        FD_ZERO(&rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 0;

        int fds[fd_count];
        int maxFd = -1;


        for (int i = 0; i < fd_count; i++) {
                int fd = socket(PF_INET, SOCK_STREAM, 0);
                if (fd < 0) printf("invalid fd in select: %d\n", fd);
                if (fd > maxFd) maxFd = fd;
                FD_SET(fd, &rfds);
                fds[i] = fd;
        }

        retval = syscall(SYS_select, maxFd + 1, &rfds, NULL, NULL, &tv);
	printf("Select returned: %d\n", retval);

        if (retval != fd_count) {
                printf("[error] select return unexpected: %d\n", retval);
        }

        for (int i = 0; i < fd_count; i++) {
                FD_CLR(fds[i], &rfds);
                int retval = close(fds[i]);
                if (retval == -1) printf ("[error] close failed in select test %d.\n", fds[i]);
        }
}
