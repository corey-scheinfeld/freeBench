#include <stdio.h>

typedef struct testInfo {
	int iter;
	const char *name;
} testInfo;


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

    if (argc != 5){printf("Invalid arguments, gave %d not 5",argc);return(0);}

	FILE *fp = argv[1];
	FILE *copy = argv[2];
    void (*f)(struct timespec*,struct timespec*) = argv[3];
    testInfo *info = argv[4];

    two_line_test(fp, copy, f, info);

}