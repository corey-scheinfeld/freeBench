#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


const char* get_val(char* line, int indx){
	char* val = strtok(line, ",");
	while(val != NULL){
		if(!--indx)
		{
			return val;
		}
		val = strtok(NULL, ",\n");
	}
	return NULL;

}

const char* find_trends(char* base, char* comp){
    
    FILE* base_file = fopen(base, "r");
    FILE* comp_file = fopen(comp, "r");
    //FILE* out_file = fopen("output.csv", "w");

    char base_line[1024];

    char comp_line[1024];

    int num_comp = 0;	

    while(fgets(base_line, 1024, base_file)){
	fgets(comp_line, 1024, comp_file);
	
	char* temp_base = strdup(base_line);
	char* temp_comp = strdup(comp_line);

	if(strcmp(get_val(temp_base, 1), get_val(temp_comp, 1)) == 0 && num_comp > 1){
		float comp_val = atof(get_val(comp_line, 2));
		float base_val = atof(get_val(base_line, 2));

		float diff = comp_val-base_val;

		float percentage = round((diff/base_val)*100);
		
		printf("Test: %s", get_val(temp_base, 1));
		printf(" Difference of %.9f, %.0f%s \n", diff, percentage, "%");
		}

	free(temp_base);
	free(temp_comp);

    	num_comp+=1;
	}

}



int main(int argc, char* argv[]){

find_trends(argv[1], argv[2]);

}
