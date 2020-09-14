#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "Intersection.h"
#include "CarManagement.h"

int main(int argc, char** argv){

	pthread_t threadC;
	int* pidSon = NULL;

	bool Auto = false;
	int nbCarMax = NB_MAX_VOITURE_DEFAULT;
	int secondaryTime = T2;

	int i;

	//Parsage des paramètres
	for(i = 1; i < argc; i++){
		if(strcmp(argv[i], "-a") == 0){
			Auto = true;
		}
		else if(strcmp(argv[i], "-t") == 0){
			if(i != (argc-1)){
				int temp = atoi(argv[i+1]);
				if(temp != 0){
					secondaryTime = temp;
				}
				i++;
			}
		}
		else if(strcmp(argv[i], "-n") == 0){
			if(i != (argc-1)){
				int temp = atoi(argv[i+1]);
				if(temp != 0){
					nbCarMax = temp;
				}
				i++;
			}
		}
	}

	// Creation of the intersection
	Intersection* intersection = init_Intersection(secondaryTime);

	if(intersection == NULL){
		return -1;
	}

	// Launch of the intersection
	pthread_create(&threadC, NULL, start_light, intersection);

	//// Launch of the car manager
	pidSon = start_manager(intersection, nbCarMax, Auto);

	// If we are in the father
	if(pidSon != NULL){
		int i;
		// We are waiting for the wires to finish
		for(i = 0; i < nbCarMax; i++){
			while(waitpid(pidSon[i], 0, 0) < 0);
		}

		// We display the end state
		printf("END : The %d cars have left\n", nbCarMax);

		// We stop the intersection
		stop_Intersection(intersection);

		pthread_join(threadC, NULL);
	}
	return 0;
}
