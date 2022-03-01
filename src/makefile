CC=cc
LDFLAGS=-pthread -O0 -lm
CFLAGS=-g

.PHONY: all
all: forkTest threadTest getpid_test read_test write_test mmap_test munmap_test\
	page_fault_test cpu_test ref_test select_test poll_test kqueue_test context_switch_test send_test recv_test

forkTest:forkTest.o OS_Eval.o
	$(CC) $(LDFLAGS) -o forkTest forkTest.o OS_Eval.o

threadTest:threadTest.o OS_Eval.o
	$(CC) $(LDFLAGS) -o threadTest threadTest.o OS_Eval.o

getpid_test:getpid_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o getpid_test getpid_test.o OS_Eval.o

cpu_test:cpu_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o cpu_test cpu_test.o OS_Eval.o

ref_test:ref_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o ref_test ref_test.o OS_Eval.o

read_test:read_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o read_test read_test.o OS_Eval.o

write_test:write_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o write_test write_test.o OS_Eval.o

mmap_test:mmap_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o mmap_test mmap_test.o OS_Eval.o

munmap_test:munmap_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o munmap_test munmap_test.o OS_Eval.o

page_fault_test:page_fault_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o page_fault_test page_fault_test.o OS_Eval.o

select_test:select_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o select_test select_test.o OS_Eval.o

poll_test:poll_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o poll_test poll_test.o OS_Eval.o

kqueue_test:kqueue_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o kqueue_test kqueue_test.o OS_Eval.o

context_switch_test:context_switch_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o context_switch_test context_switch_test.o OS_Eval.o

send_test:send_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o send_test send_test.o OS_Eval.o

recv_test:recv_test.o OS_Eval.o
	$(CC) $(LDFLAGS) -o recv_test recv_test.o OS_Eval.o

.PHONY: clean
clean:
	rm -rf OS_Eval forkTest threadTest getpid_test read_test write_test mmap_test munmap_test\
	page_fault_test cpu_test ref_test select_test poll_test kqueue_test context_switch_test send_test recv_test\
	OS_Eval.o forkTest.o threadTest.o getpid_test.o read_test.o write_test.o mmap_test.o munmap_test.o\
	page_fault_test.o cpu_test.o ref_test.o select_test.o poll_test.o kqueue_test.o context_switch_test.o\
	send_test.o recv_test.o
