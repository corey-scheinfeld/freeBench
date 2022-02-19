CC=gcc
LDFLAGS=-pthread -O0 -lm
CFLAGS=-g

.PHONY: all
all: forkTest threadTest

forkTest:forkTest.o OS_Eval.o
	$(CC) $(LDFLAGS) -o forkTest forkTest.o OS_Eval.o

threadTest:threadTest.o OS_Eval.o
	$(CC) $(LDFLAGS) -o threadTest threadTest.o OS_Eval.o


.PHONY: clean
clean:
	rm -rf OS_Eval forkTest threadTest getpid_test read_test write_test mmap_test munmap_test\
	page_fault_test cpu_test ref_test select_test poll_test kqueue_test context_switch_test send_test recv_test\
	OS_Eval.o forkTest.o threadTest.o getpid_test.o read_test.o write_test.o mmap_test.o munmap_test.o\
	page_fault_test.o cpu_test.o ref_test.o select_test.o poll_test.o kqueue_test.o context_switch_test.o\
	send_test.o recv_test.o
