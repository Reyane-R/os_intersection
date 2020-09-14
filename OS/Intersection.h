#ifndef INTERSECTION
#define INTERSECTION

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "ipcTools.h"


#define T 1000
#define T2 500
#define NB_MAX_VOITURE_DEFAULT 10

// Structure of the intersection
typedef struct Intersection Intersection;
struct Intersection{
	int* muxRoad;
	int muxintersection;
	int* light;
	key_t keyLight;
	bool* stop;
	int* secondaryTime;
};

// Creation of the intersection
Intersection* init_Intersection(int secondaryTime);

// Intersection stop
void stop_Intersection(Intersection* intersection);

// Release the intersection
void free_Intersection(Intersection* intersection);

// Launch of the intersection (in thread)
void* start_light(void* infos);

// Display of intersection messages
void print_Intersection_message(char* message);

#endif
