#ifndef CAR
#define CAR

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ipcTools.h"
#include "Intersection.h"

#define WAIT 20
#define TIME_IN_CARR 100

//Structure of a car
typedef struct Car Car;
struct Car{
		int id;
		int muxRoad;
		int muxintersection;
		int* light;
		int road;
};

//Creation of a car
Car* start_car(int id, int road, Intersection* intersection, int* pid);

//Reaching the intersection
void enterintersection(Car* car);

//status of the cars
void print_car_message(char* message);

#endif

#ifndef CARMAN
#define CARMAN

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>
#include "Intersection.h"
#include "ipcTools.h"
#include "Car.h"

#define REACH_MAX 1000000
#define REACH_MIN 1000

//Car creation manager function
int* start_manager(Intersection* intersection, int nbCarMax, bool Auto);

#endif
