/*
Computer Operating Systems HW3
Date: 01.06.2023
*/

#include <stdio.h>

#define MAX_PROCESSES 100
#define MAX_RESOURCES 100

int main() {
	int requests[MAX_PROCESSES][MAX_RESOURCES];
	int allocations[MAX_PROCESSES][MAX_RESOURCES];
	int resources[MAX_RESOURCES];
	FILE* request_file = fopen("requests.txt", "r");
    if (request_file == NULL) 
    {
        printf("Error opening the file.\n");
        return 1;
    }

    int num_process = 0;
    int num_resources = 0;
    char ch;

    while ((ch = fgetc(request_file)) != EOF) {
        if (ch == '\n') {
            num_process++; //Find the number of processes
        }
    }
    // Reset the file pointer to the beginning
    fseek(request_file, 0, SEEK_SET);

    while ((ch = fgetc(request_file)) != EOF && ch != '\n') {
        if (ch != ' ') {
            num_resources++; //Find the number of resources
        }
    }
	fseek(request_file, 0, SEEK_SET);
    
	for(int i=0; i<num_process+1; i++){
		for (int j=0 ; j<num_resources; j++)
		{
			int numRead = fscanf(request_file, "%d", &requests[i][j]); //Read the requests.txt to requests array

			if (numRead != 1) {
            printf("Error reading allocations.txt\n");
            return 1;
			}
		}
	}

	fclose(request_file);

	FILE* alloc_file = fopen("allocations.txt", "r");
    if (alloc_file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }
    for(int i=0; i<num_process+1; i++){
		for (int j=0 ; j<num_resources; j++){
			int numRead = fscanf(alloc_file, "%d", &allocations[i][j]); //Read the allocations.txt to allocations array
			if (numRead != 1) {
				printf("Error reading allocations.txt\n");
				return 1;
			}
		}
	}
	fclose(alloc_file);

    FILE* resource_file = fopen("resources.txt", "r"); //Resource file is opened 
    if (resource_file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }
    
		for (int j=0 ; j<num_resources; j++){
			int numRead = fscanf(resource_file, "%d", &resources[j]); //Read the resources.txt to 2d resources array
			if (numRead != 1) {
            printf("Error reading allocations.txt\n");
            return 1;
        }
	}
	fclose(resource_file);
    //All files have been read
    
    num_process= num_process+1;
    
	for(int i=0; i<num_process; i++){ //Print the information for all processes
		   
		printf("Information  for process :P%d: \n", i+1);
		printf("Allocated Resources: ");
		for(int j=0; j<num_resources; j++){
			printf("R%d:%d ", j+1,allocations[i][j]);
	    }
	    printf("\n");
	    printf("Resource Request: ");
		for(int j=0; j<num_resources; j++){
			printf("R%d:%d ", j+1,requests[i][j]);
	    }
	    printf("\n");
	        printf("\n");
		} 
	
    //Find the available resources that can be allocated
    int available[num_resources];
    for(int a=0; a<num_resources; a++)
    {
		int total=0;
		for(int k=0; k<num_process+1; k++)
		{
			total= total + allocations[k][a];
		}
		available[a]= resources[a] - total;
		}
   
    int num_finished=0;
    int finish[num_process]; //Create an array for keeping finish flag for processses
    int finished_list[num_process];
    for (int i=0; i< num_process; i++)
    {
		 finish[i]=0; //Processes not finished initally.	 
	}	
		
	for (int i=0; i< num_process; i++){
		int enough=0;
		if (finish[i]==0){ //If a process is not finished, we check availablity of the resources it requests
			for (int k=0; k<num_resources; k++){
				if(requests[i][k] > available[k])
				{
					 enough=1; //If there is at least one request that is bigger than the available resource, process is not ready to run
					 break;
				}
			}
			if(enough==0) //If process is ready to run
			{
				finish[i]=1;
				finished_list[num_finished]=i+1;
				num_finished++; //Increment the number of finished process
				for(int j=0; j<num_resources; j++)
				{
					available[j]+= allocations[i][j]; //We add the resources a finishing process holds to the available resources
				}
				i=-1 ; //Return to the beginning to check from the beginning
			}
				  
		}
		 
	}
	
	if(num_finished!=0)
	{
		printf("Running order for processses: ");
		for(int i=0; i<num_finished ; i++){
			printf(" P%d ", finished_list[i]);
		}
		printf("\n");
		if(num_finished!=num_process){
			printf("There is a deadlock. Processes that causes the deadlock: ");
		}
		for(int a=0 ; a<num_process; a++){
			if(finish[a]==0){
				printf(" P%d ", a+1);
				}			
		}
        printf("\n");
    
	}
    return 0;
}


