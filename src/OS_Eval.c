#include "OS_Eval.h"

char *home = "/home/corey/freeBench/src/";

int file_size = -1;
int fd_count = -1;
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
        {10, "base"},
        {1000, "big"},
	{-1, ""}
};

msgSpec msg_tests[] = {
        {1, 50, "base"},
        {96000, 1,  "big"},
        {-1, -1, ""}
};


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

		printf("      %s time:, iter %d,  %ld.%09ld,\n", info->name, (i+1),timeArray[i].tv_sec, timeArray[i].tv_nsec);
	}

	free(timeArray);

	clock_gettime(CLOCK_MONOTONIC,&testEnd);
	struct timespec *diffTime = calc_diff(&testStart, &testEnd);
	printf("      %s test total runtime:, , %ld.%09ld\n",info->name, diffTime->tv_sec, diffTime->tv_nsec); 
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

