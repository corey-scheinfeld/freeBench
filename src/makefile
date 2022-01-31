CC=gcc
LDFLAGS=-pthread -O0 -lm
CFLAGS=-g
OBJECTS=OS_Eval.o forkTest.o threadTest.o getpid_test.o read_test.o write_test.o mmap_test.o munmap_test.o page_fault_test.o
TARGET=OS_Eval

all: $(TARGET)

$(TARGET):$(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf OS_Eval forkTest threadTest getpid_test read_test write_test mmap_test munmap_test\
	page_fault_test\
	OS_Eval.o forkTest.o threadTest.o getpid_test.o read_test.o write_test.o mmap_test.o munmap_test.o\
	page_fault_test.o
