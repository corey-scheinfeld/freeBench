#include "OS_Eval.h"

char *home = "/home/corey/freeBench/src/";

int file_size = -1;
int fd_count = -1;
int msg_size = -1;
int curr_iter_limit = -1;


int counter=3;
bool  isFirstIteration = false;

char *output_fn = NULL;
char *new_output_fn = NULL;

#define setup 		(struct timespec *fp) \
			{struct timespec timeA ; \
			struct timespec timeC; \
			clock_gettime(CLOCK_MONOTONIC,&timeA);

#define setup_2_a	(struct timespec *fp) \
			{struct timespec timeA;	\
			struct timespec timeC;

#define setup_2_b	clock_gettime(CLOCK_MONOTONIC,&timeA);

#define ending		clock_gettime(CLOCK_MONOTONIC,&timeC); \
			add_diff_to_sum(fp,timeC,timeA);\
			return;}

#define end1	clock_gettime(CLOCK_MONOTONIC,&timeC);

#define end2	add_diff_to_sum(fp,timeC,timeA);\
				return;}

#define final_formula ",=\"AVERAGE(B%d:INDIRECT(SUBSTITUTE(ADDRESS(1,COLUMN()-1,4),\"1\",\"%d\")))\",=\"STDEV.P(B%d:INDIRECT(SUBSTITUTE(ADDRESS(1,COLUMN()-2,4),\"1\",\"%d\")))\"\n",counter,counter,counter,counter

#define OUTPUT_FILE_PATH	""
#define OUTPUT_FN		OUTPUT_FILE_PATH "output_file.csv"
#define NEW_OUTPUT_FN	OUTPUT_FILE_PATH "new_output_file.csv"
#define BASE_ITER 10000

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

struct timespec *calc_sum(struct timespec ** array, int size)
{
	
	struct timespec *sum = (struct timespec *)malloc(sizeof(struct timespec));
	sum->tv_sec = 0;
	sum->tv_nsec = 0;
	for (int i = 0; i < size; i ++)
	{
		if (array[i]->tv_nsec >= (1000000000 - sum->tv_nsec)) 
		{
			sum->tv_sec = sum->tv_sec +1;
			sum->tv_nsec = array[i]->tv_nsec - (1000000000 - sum->tv_nsec);
			sum->tv_sec = sum->tv_sec + array[i]->tv_sec;
		} else {
		
			sum->tv_nsec = sum->tv_nsec + array[i]->tv_nsec;
			sum->tv_sec = sum->tv_sec + array[i]->tv_sec;
		}
	}	
	return sum;
}

struct timespec *calc_sum2(struct timespec * array, int size)
{
	
	struct timespec *sum = (struct timespec *)malloc(sizeof(struct timespec));
	sum->tv_sec = 0;
	sum->tv_nsec = 0;
	for (int i = 0; i < size; i ++)
	{
		if (array[i].tv_nsec >= (1000000000 - sum->tv_nsec)) 
		{
			sum->tv_sec = sum->tv_sec +1;
			sum->tv_nsec = array[i].tv_nsec - (1000000000 - sum->tv_nsec);
			sum->tv_sec = sum->tv_sec + array[i].tv_sec;
		} else {
		
			sum->tv_nsec = sum->tv_nsec + array[i].tv_nsec;
			sum->tv_sec = sum->tv_sec + array[i].tv_sec;
		}
	}	
	return sum;
}

int comp(const void *ele1, const void *ele2) 
{
	struct timespec t1 = *((struct timespec *)ele1);
	struct timespec t2 = *((struct timespec *)ele2);
	if (t1.tv_sec > t2.tv_sec) {
		return 1;
	}
	else if (t1.tv_sec == t2.tv_sec) {
		if (t1.tv_nsec > t2.tv_nsec) return 1;
		else if (t1.tv_nsec == t2.tv_nsec) return 0;
		else return -1;
	} 
	else {
		return -1;
	}
}

#define INPRECISION 0.05
#define K 5
struct timespec *calc_k_closest(struct timespec *timeArray, int size)
{
	if(DEBUG) printf("in calc_k_closest\n");
	qsort(timeArray, size, sizeof(struct timespec), comp);
	struct timespec **k_closest = (struct timespec **) malloc (sizeof(struct timespec*) * K);
	for (int ii = 0; ii < K; ii++) 
		k_closest[ii] = NULL;
	struct timespec *prev = &timeArray[0];
	int j = 0;
	k_closest[j] = prev;
	j ++;
	for (int i = 1; i < size; i ++) 
	{
		struct timespec *curr = &timeArray[i];
		if(DEBUG) printf("curr %ld.%09ld\n",curr->tv_sec, curr->tv_nsec); 
		if (curr->tv_sec != 0 || prev->tv_sec != 0) {
			if(DEBUG) printf("[warn] test run greater than 1 second: ");
			if(DEBUG) printf("prev %ld.%09ld, ",prev->tv_sec, prev->tv_nsec); 
			if(DEBUG) printf("curr %ld.%09ld\n",curr->tv_sec, curr->tv_nsec); 
			j = 0;
		} else {
			double diff = curr->tv_nsec - prev->tv_nsec;
			double ratioDiff = diff/(double)prev->tv_nsec;
			if(DEBUG) printf("diff: %lf\n", ratioDiff);
			if (ratioDiff > INPRECISION)
			{	
				j = 0;
				for (int ii = 0; ii < K; ii++) 
					k_closest[ii] = NULL;
			}
			else 
			{
				k_closest[j] = curr;
				j ++;	

			}
		}
		if (j == K) break;
		prev = curr;
	}
	if (DEBUG && j != K) printf("only found the %d closest\n", j);
	struct timespec* result = k_closest[0];
	if(DEBUG) printf("result %ld.%09ld\n", result->tv_sec, result->tv_nsec); 
	free(k_closest);
	return result;

}

struct timespec *calc_min_max(struct timespec *timeArray, int size, bool comp_type)
{

	struct timespec *val = (struct timespec *)malloc(sizeof(struct timespec));

	if(DEBUG) printf("in calc_min_max\n");
	qsort(timeArray, size, sizeof(struct timespec), comp);

	if(comp_type){
	//true for min
		val->tv_sec = timeArray[0].tv_sec;
		val->tv_nsec = timeArray[0].tv_nsec;
	} else {
	//false for max
		val->tv_sec = timeArray[size-1].tv_sec;
		val->tv_nsec = timeArray[size-1].tv_nsec;
	}

	return val;
}

void calc_sq_diff(struct timespec *time, struct timespec *average, struct timespec *sq_diff){

	if ((time->tv_nsec - average->tv_nsec) < 0) {
		sq_diff->tv_sec = time->tv_sec - average->tv_sec - 1;
		sq_diff->tv_nsec = time->tv_nsec - average->tv_nsec + 1000000000;
	} else {
		sq_diff->tv_sec = time->tv_sec - average->tv_sec;
		sq_diff->tv_nsec = time->tv_nsec - average->tv_nsec;
	}

}

struct timespec *calc_variance(struct timespec *timeArray, int size, struct timespec *average){
	
	if(DEBUG) printf("in calc_variance\n");

	struct timespec* sqDiffArray = (struct timespec *)malloc(sizeof(struct timespec) * size);
		
	for (int i = 0; i < size; i ++) 
	{
		struct timespec *curr = &timeArray[i];

		calc_sq_diff(curr, average, &sqDiffArray[i]);		
	}

	struct timespec *sum = calc_sum2(sqDiffArray, size);
	struct timespec *variance = calc_average(sum, size);

	if(DEBUG) printf("result %ld.%09ld\n", variance->tv_sec, variance->tv_nsec);
	
	free(sqDiffArray);
	free(sum);
	return variance;
}

struct timespec *calc_stddev(struct timespec *timeArray, int size, struct timespec *variance){
	
	if(DEBUG) printf("in calc_stddev\n");

	struct timespec *stddev = (struct timespec *)malloc(sizeof(struct timespec));
	
	double variance_fp  = (double)(variance->tv_sec) + (double)(variance->tv_nsec / 1000000000);
	
	double stddev_fp  = sqrt(variance_fp); 

	stddev->tv_nsec = modf(stddev_fp, (double *)&stddev->tv_sec); 
 
	if(DEBUG) printf("result %ld.%09ld\n", stddev->tv_sec, stddev->tv_nsec);
	
	return stddev;
}

void one_line_test(FILE *fp, FILE *copy, void (*f)(struct timespec*), testInfo *info){
	struct timespec testStart, testEnd;
	clock_gettime(CLOCK_MONOTONIC,&testStart);

	printf("Performing test %s.\n", info->name);

	int runs = info->iter;
	printf("Total test iteration %d.\n", runs);

	struct timespec* timeArray = (struct timespec *)malloc(sizeof(struct timespec) * runs);
	for (int i=0; i < runs; i++) {
		timeArray[i].tv_sec = 0;
		timeArray[i].tv_nsec = 0;
		(*f)(&timeArray[i]);
	}
	struct timespec *sum = calc_sum2(timeArray, runs);
	struct timespec *average = calc_average(sum, runs);  
	struct timespec *kbest = calc_k_closest(timeArray, runs);	

	if (!isFirstIteration)
	{
		char ch;
		while (1)
		{
			ch=fgetc(copy);
			if (ch == '\n')	break;
			fputc(ch,fp);
		}
	} else {
		fprintf(fp, "%10s", info->name);
		fprintf(fp, "          kbest:,");
	}
	fprintf(fp,"%ld.%09ld,\n",kbest->tv_sec, kbest->tv_nsec); 

	if (!isFirstIteration)
	{
		char ch;
		while (1)
		{
			ch=fgetc(copy);
			if (ch == '\n')	break;
			fputc(ch,fp);
		}
	} else {
		fprintf(fp, "%10s", info->name);
		fprintf(fp, "        average:,");
	}
	fprintf(fp,"%ld.%09ld,\n",average->tv_sec, average->tv_nsec); 

	free(sum);
	free(average);
	free(timeArray);

	clock_gettime(CLOCK_MONOTONIC,&testEnd);
	struct timespec *diffTime = calc_diff(&testStart, &testEnd);
	printf("Test took: %ld.%09ld seconds\n",diffTime->tv_sec, diffTime->tv_nsec); 
	free(diffTime);

	return;
}

void one_line_test_v2(FILE *fp, FILE *copy, void (*f)(struct timespec*, int, int *), testInfo *info){
	struct timespec testStart, testEnd;
	clock_gettime(CLOCK_MONOTONIC,&testStart);

	printf("Performing test %s.\n", info->name);

	int runs = info->iter;
	printf("Total test iteration %d.\n", runs);

	struct timespec* timeArray = (struct timespec *)malloc(sizeof(struct timespec) * runs);

	for (int i = 0; i < runs; i++) {
		timeArray[i].tv_sec = 0;
		timeArray[i].tv_nsec = 0;
	}

	for (int i = 0; i < runs; ) {
		(*f)(timeArray, info->iter, &i);
	}

	struct timespec *sum = calc_sum2(timeArray, runs);
	struct timespec *average = calc_average(sum, runs);  
	struct timespec *kbest = calc_k_closest(timeArray, runs);	

	if (!isFirstIteration)
	{
		char ch;
		while (1)
		{
			ch=fgetc(copy);
			if (ch == '\n')	break;
			fputc(ch,fp);
		}
	} else {
		fprintf(fp, "%10s", info->name);
		fprintf(fp, "          kbest:,");
	}
	fprintf(fp,"%ld.%09ld,\n",kbest->tv_sec, kbest->tv_nsec); 

	if (!isFirstIteration)
	{
		char ch;
		while (1)
		{
			ch=fgetc(copy);
			if (ch == '\n')	break;
			fputc(ch,fp);
		}
	} else {
		fprintf(fp, "%10s", info->name);
		fprintf(fp, "        average:,");
	}
	fprintf(fp,"%ld.%09ld,\n",average->tv_sec, average->tv_nsec); 

	free(sum);
	free(average);
	free(timeArray);


	clock_gettime(CLOCK_MONOTONIC,&testEnd);
	struct timespec *diffTime = calc_diff(&testStart, &testEnd);
	printf("Test took: %ld.%09ld seconds\n",diffTime->tv_sec, diffTime->tv_nsec); 
	free(diffTime);

	return;
}

void two_line_test(FILE *fp, FILE *copy, void (*f)(struct timespec*,struct timespec*), testInfo *info){
	struct timespec testStart, testEnd;
	clock_gettime(CLOCK_MONOTONIC,&testStart);
	printf("Performing test %s.\n", info->name);

	int runs = info->iter;
	printf("Total test iteration %d.\n", runs);
	struct timespec* timeArrayParent = (struct timespec *) malloc(sizeof(struct timespec) * runs);
	struct timespec* timeArrayChild = (struct timespec *) malloc(sizeof(struct timespec) * runs);
	for (int i=0; i < runs; i++)
	{
		timeArrayParent[i].tv_sec = 0;
		timeArrayParent[i].tv_nsec = 0;
		timeArrayChild[i].tv_sec = 0;
		timeArrayChild[i].tv_nsec = 0;
		(*f)(&timeArrayChild[i],&timeArrayParent[i]);
	}

	struct timespec *sumParent = calc_sum2(timeArrayParent, runs);
	struct timespec *sumChild = calc_sum2(timeArrayChild, runs);
	struct timespec *averageParent = calc_average(sumParent, runs);
	struct timespec *averageChild = calc_average(sumChild, runs);
	struct timespec **averages = (struct timespec **) malloc(2*sizeof(struct timespec *));
	averages[0] = averageParent;
	averages[1] = averageChild;

	struct timespec *kbestParent = calc_k_closest(timeArrayParent, runs);
	struct timespec *kbestChild = calc_k_closest(timeArrayChild, runs);
	struct timespec **kbests = (struct timespec **) malloc(2*sizeof(struct timespec *));
	kbests[0] = kbestParent;
	kbests[1] = kbestChild;

	char ch;
	if(!isFirstIteration)
	{
		for (int i=0; i<2; i++)
		{
			while(1)
			{
				ch=fgetc(copy);
				if(ch=='\n')
					break;
				fputc(ch,fp);
			}
			fprintf(fp,"%ld.%09ld,\n",kbests[i]->tv_sec, kbests[i]->tv_nsec); 

			while(1)
			{
				ch=fgetc(copy);
				if(ch=='\n')
					break;
				fputc(ch,fp);
			}
			fprintf(fp,"%ld.%09ld,\n",averages[i]->tv_sec,averages[i]->tv_nsec); 
		}
	}
	else
	{
		fprintf(fp, "%10s", info->name);
		fprintf(fp,"          kbest:,%ld.%09ld,\n",kbests[0]->tv_sec, kbests[0]->tv_nsec); 
		fprintf(fp, "%10s", info->name);
		fprintf(fp,"       average:,%ld.%09ld,\n",averages[0]->tv_sec, averages[0]->tv_nsec); 

		fprintf(fp, "%10s", info->name);
		fprintf(fp,"    Child kbest:,%ld.%09ld,\n",kbests[1]->tv_sec, kbests[1]->tv_nsec); 
		fprintf(fp, "%10s", info->name);
		fprintf(fp," Child average:,%ld.%09ld,\n",averages[1]->tv_sec, averages[1]->tv_nsec); 
	}
	free(timeArrayChild);
	free(timeArrayParent);
	free(averages);
	free(kbests);
	free(sumParent);
	free(sumChild);
	free(averageParent);
	free(averageChild);

	clock_gettime(CLOCK_MONOTONIC,&testEnd);
	struct timespec *diffTime = calc_diff(&testStart, &testEnd);
	printf("Test took: %ld.%09ld seconds\n",diffTime->tv_sec, diffTime->tv_nsec); 
	free(diffTime);
	return;
}

int main(int argc, char *argv[])
{
	output_fn = (char *)malloc(500*sizeof(char));
	strcpy(output_fn, home);
	strcat(output_fn, OUTPUT_FN);

	new_output_fn = (char *)malloc(500*sizeof(char));
	char *res = strcpy(new_output_fn, home);
	strcat(new_output_fn, NEW_OUTPUT_FN);

	struct timespec startTime, endTime;
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	if (argc != 3){printf("Invalid arguments, gave %d not 3",argc);return(0);}
	char *iteration = argv[1];
	char *str_os_name = argv[2];
	FILE *fp;
	FILE *copy = NULL;
	fp=fopen(new_output_fn,"w");
	isFirstIteration = false;
	if (*iteration == '0'){isFirstIteration = true;}
	if (!isFirstIteration)
	{
		copy=fopen(output_fn,"r");
		char ch;
		int increment = 0;
		while (1)
		{
			ch=fgetc(copy);
			if (ch == '\n')
			{
				if (increment == 1)
				{
					break;
				}
				increment ++;
			}
			fputc(ch,fp);
		}
	}
	else
	{
		fprintf(fp, "OS Benchmark experiment\nTest Name:,");
	}
	fprintf(fp,"%s,\n",str_os_name);
	
	testInfo info;	
	
	/*****************************************/
	/*               GETPID                  */
	/*****************************************/
	
	sleep(60);
	info.iter = BASE_ITER * 100;
	info.name = "ref";
	one_line_test(fp, copy, ref_test, &info);

	info.iter = 100;
	info.name = "cpu";
	one_line_test(fp, copy, cpu_test, &info);

	info.iter = BASE_ITER * 100;
	info.name = "getpid";
	one_line_test(fp, copy, getpid_test, &info);
	
	/*****************************************/
	/*            CONTEXT SWITCH             */
	/*****************************************/
	
	info.iter = BASE_ITER * 10;
	info.name = "context siwtch";
	one_line_test(fp, copy, context_switch_test, &info);
	
	/*****************************************/
	/*             SEND & RECV               */
	/*****************************************/
	
	msg_size = 1;	
	curr_iter_limit = 50;
	printf("msg size: %d.\n", msg_size);
	printf("curr iter limit: %d.\n", curr_iter_limit);
	info.iter = BASE_ITER * 10;
	info.name = "send";
	one_line_test_v2(fp, copy, send_test, &info);
	
	info.iter = BASE_ITER * 10;
	info.name = "recv";
	one_line_test_v2(fp, copy, recv_test, &info);
	

	msg_size = 96000;	// This size 96000 would cause blocking on older kernels!
	curr_iter_limit = 1;
	printf("msg size: %d.\n", msg_size);
	printf("curr iter limit: %d.\n", curr_iter_limit);
	info.iter = BASE_ITER;
	info.name = "big send";
	one_line_test_v2(fp, copy, send_test, &info);
		
	info.iter = BASE_ITER;
	info.name = "big recv";
	one_line_test_v2(fp, copy, recv_test, &info);
	
	/*****************************************/
	/*         FORK & THREAD CREATE          */
	/*****************************************/
	
	info.iter = BASE_ITER * 2;
	info.name = "fork";
	two_line_test(fp, copy, forkTest, &info);
	
	info.iter = BASE_ITER * 5;
	info.name = "thr create";
	two_line_test(fp, copy, threadTest, &info);


	int page_count = 6000;
	void *pages[page_count];
	for (int i = 0; i < page_count; i++) {
    		pages[i] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	}
	
	info.iter = BASE_ITER / 2;	
	info.name = "big fork";
	two_line_test(fp, copy, forkTest, &info);

	for (int i = 0; i < page_count; i++) {
		munmap(pages[i], PAGE_SIZE);
	}

	page_count = 12000;
	printf("Page count: %d.\n", page_count);
	void *pages1[page_count];
	for (int i = 0; i < page_count; i++) {
    		pages1[i] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	}
	
	info.iter = BASE_ITER / 2;	
	info.name = "huge fork";
	two_line_test(fp, copy, forkTest, &info);

	for (int i = 0; i < page_count; i++) {
		munmap(pages1[i], PAGE_SIZE);
	}
	
	/*****************************************/
	/*     WRITE & READ & MMAP & MUNMAP      */
	/*****************************************/

	/****** SMALL ******/
	file_size = PAGE_SIZE;	
	printf("file size: %d.\n", file_size);

	info.iter = BASE_ITER * 10;
	info.name = "small write";
	one_line_test(fp, copy, write_test, &info);

	info.iter = BASE_ITER * 10; 
	info.name = "small read";
	read_warmup();
	one_line_test(fp, copy, read_test, &info);

	info.iter = BASE_ITER * 10;
	info.name = "small mmap";
	one_line_test(fp, copy, mmap_test, &info);
	
	info.iter = BASE_ITER * 10;
	info.name = "small munmap";
	one_line_test(fp, copy, munmap_test, &info);
	
	info.iter = BASE_ITER * 5;
	info.name = "small page fault";
	one_line_test(fp, copy, page_fault_test, &info);

	
	/****** MID ******/
	
	file_size = PAGE_SIZE * 10;
	printf("file size: %d.\n", file_size);

	info.iter = BASE_ITER * 10;
	info.name = "mid write";
	one_line_test(fp, copy, write_test, &info);

	info.iter = BASE_ITER * 10;
	info.name = "mid read";
	read_warmup();
	one_line_test(fp, copy, read_test, &info);
	
	info.iter = BASE_ITER * 10;
	info.name = "mid mmap";
	one_line_test(fp, copy, mmap_test, &info);
	
	info.iter = BASE_ITER * 10;
	info.name = "mid munmap";
	one_line_test(fp, copy, munmap_test, &info);
	
	info.iter = BASE_ITER * 5;
	info.name = "mid page fault";
	one_line_test(fp, copy, page_fault_test, &info);
	

	/****** BIG ******/
	
	file_size = PAGE_SIZE * 1000;	
	printf("file size: %d.\n", file_size);

	info.iter = BASE_ITER / 2;
	info.name = "big write";
	one_line_test(fp, copy, write_test, &info);
	
	info.iter = BASE_ITER;
	info.name = "big read";
	read_warmup();
	one_line_test(fp, copy, read_test, &info);
	
	info.iter = BASE_ITER * 10;
	info.name = "big mmap";
	one_line_test(fp, copy, mmap_test, &info);
	
	info.iter = BASE_ITER / 4;
	info.name = "big munmap";
	one_line_test(fp, copy, munmap_test, &info);

	info.iter = BASE_ITER * 5;
	info.name = "big page fault";
	one_line_test(fp, copy, page_fault_test, &info);
	

        /****** HUGE ******/
	
	file_size = PAGE_SIZE * 10000;	
	printf("file size: %d.\n", file_size);

	info.iter = BASE_ITER / 4;
	info.name = "huge write";
	one_line_test(fp, copy, write_test, &info);
	
	info.iter = BASE_ITER;
	info.name = "huge read";
	one_line_test(fp, copy, read_test, &info);
	
	info.iter = BASE_ITER * 10;
	info.name = "huge mmap";
	one_line_test(fp, copy, mmap_test, &info);
	
	info.iter = BASE_ITER / 4; 
	info.name = "huge munmap";
	one_line_test(fp, copy, munmap_test, &info);
	
	info.iter = BASE_ITER * 5;
	info.name = "huge page fault";
	one_line_test(fp, copy, page_fault_test, &info);

	/*****************************************/
	/*              WRITE & READ             */
	/*****************************************/
		
	/****** SMALL ******/
	
	fd_count = 10;

	info.iter = BASE_ITER * 10;
	info.name = "select";
	one_line_test(fp, copy, select_test, &info);
		
	info.iter = BASE_ITER * 10;
	info.name = "poll";
	one_line_test(fp, copy, poll_test, &info);
	
	info.iter = BASE_ITER * 10;
	info.name = "kqueue";
	one_line_test(fp, copy, kqueue_test, &info);
	
	/****** BIG ******/
	
	fd_count = 1000;

	info.iter = BASE_ITER;
	info.name = "select big";
	one_line_test(fp, copy, select_test, &info);

	info.iter = BASE_ITER;
	info.name = "poll big";
	one_line_test(fp, copy, poll_test, &info);
	
	info.iter = BASE_ITER;
	info.name = "kqueue big";
	one_line_test(fp, copy, kqueue_test, &info);
	
	fclose(fp);
	if (!isFirstIteration)
	{
		fclose(copy);
		remove(output_fn);
	}
	char name[300];
	strcpy(name, home);
	strcat(name, OUTPUT_FILE_PATH);
	strcat(name, "output.");
	strcat(name, str_os_name);
	strcat(name, ".csv");
	strcat(name, "\0");
	int ret = rename(new_output_fn,name);
	clock_gettime(CLOCK_MONOTONIC, &endTime);
	struct timespec *diffTime = calc_diff(&startTime, &endTime);
	printf("Test took: %ld.%09ld seconds\n",diffTime->tv_sec, diffTime->tv_nsec); 
	free(diffTime);
	remove("test_file.txt");
	return(0);
}
