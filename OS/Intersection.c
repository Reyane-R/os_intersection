#include "Intersection.h"
#include "Car.h"
//Creation of the Intersection
Intersection* init_Intersection(int secondaryTime){
	Intersection* c = (Intersection*) malloc(sizeof(Intersection));

	//Generation of the keys
	key_t keymuxintersection = ftok("/etc/passwd", 0);
	key_t keyMuxRoad0 = ftok("/etc/passwd", 1);
	key_t keyMuxRoad1 = ftok("/etc/passwd", 2);
	c->keyLight = ftok("/etc/passwd", 3);

	//Allocation of the  lights
	c->light = shmalloc(c->keyLight, sizeof(int));
	if(c->light == (int*) -1){
		perror("Error in creating the light");
		return NULL;
	}
	*c->light = -1;

	//Allocation of the stop
	c->stop = malloc(sizeof(bool));
	*c->stop = false;

	//Allocation of the mutex for the intersection
	c->muxintersection = mutalloc(keymuxintersection + 1);

	//Mutex allocation for each road
	c->muxRoad = malloc(2*sizeof(int));
	c->muxRoad[0] = mutalloc(keyMuxRoad0 + 1);
	c->muxRoad[1] = mutalloc(keyMuxRoad1 + 1);
	if((c->muxRoad[0] == -1) || (c->muxRoad[1] == -1)){
		perror("Erreur dans la création des voies");
		return NULL;
	}

	//Time allocation of the secondary road
	c->secondaryTime = malloc(sizeof(int));
	*c->secondaryTime = secondaryTime;

	return c;
}

//Intersection stop
void stop_Intersection(Intersection* Intersection){
	*Intersection->stop = true;
}

//Liberation of the Intersection
void free_Intersection(Intersection* Intersection){
	free(Intersection->stop);
	shmfree(Intersection->keyLight, Intersection->light);
	mutfree(Intersection->muxintersection);
	mutfree(Intersection->muxRoad[0]);
	mutfree(Intersection->muxRoad[1]);
	free(Intersection);
}

//Intersection launch (in thread)
void* start_light(void* infos){
	Intersection* intersection = (Intersection*) infos;
	int* light = intersection->light;
	bool* stop = intersection->stop;
	char* message = malloc(100*sizeof(char));

	//Light initialization
	*light = 0;
	sprintf(message, "The light %d turns red", !*light);
	print_Intersection_message(message);
	sprintf(message, "The light %d turns green", *light);
	print_Intersection_message(message);

	// Until we hit the Intersection
	while(!*stop){
		int i;

		// We allocate the waiting time
		if(*light == 0){
			i = T*1000;
		}
		else{
			i = *intersection->secondaryTime*1000;
		}

		// We are waiting for it
		do{
			usleep(100);
			i -= 100;
		}while(i > 0 && !*stop);

		// And change of light
		if(!*stop){
			sprintf(message, "The light %d turns red", *light);
			print_Intersection_message(message);
			sprintf(message, "Le light %d turns green", !*light);
			print_Intersection_message(message);
			*light = !*light;
		}
	}

	pthread_exit(NULL);
}

// Display of messages from the Intersection
void print_Intersection_message(char* message){
	printf("Intersection : %s\n", message);
}
//Car creation
Car* start_car(int id, int road, Intersection* intersection, int* pid){

	//We fork and fill the structure if it's the son
	if((*pid = fork()) == 0){
		Car* v = malloc(sizeof(Car));
		v->light = intersection->light;
		v->muxRoad = intersection->muxRoad[road];
		v->muxintersection = intersection->muxintersection;
		v->road = road;
		v->id = id;
		return v;
	}
	//Or we return null if it's the father
	else{
		return NULL;
	}

}

//Entrance into the intersection
void enterintersection(Car* car){

	int* light = car->light;
	int muxRoad = car->muxRoad;
	int muxintersection = car->muxintersection;
	char* message = malloc(100*sizeof(char));
	sprintf(message, "Car %d has in on road %d", car->id, car->road);
	print_car_message(message);

	//We request access to the first place at the  light
	P(muxRoad);
	sprintf(message, "Car %d reached the intersection", car->id);
	print_car_message(message);

	//We are waiting for the light to turn green
	while(*light != car->road){
		usleep(WAIT*1000);
	}

	sprintf(message, "Car %d is occupied", car->id);
	print_car_message(message);

	//We ask for the intersection
	P(muxintersection);
	sprintf(message, "Car %d is waiting", car->id);
	print_car_message(message);

	//We free the first place of the light
	V(muxRoad);

	//We pass
	usleep(TIME_IN_CARR*1000);
	sprintf(message, "Car %d has left", car->id);
	print_car_message(message);

	//we free the intersection
	V(muxintersection);

	//We continue
	free(car);
}

//Affichage des messages des cars
void print_car_message(char* message){
	printf("\tCAR : %s\n", message);
}


//Car creation manager function
int* start_manager(Intersection* intersection, int nbCarMax, bool Auto){

	int nbCar = 0;

	int* pidSon = malloc(nbCarMax*sizeof(int));

	//If in automatic mode
	if(Auto){
		srand(time(NULL));
		//As long as we have not exceeded the number of coaches required
		while(nbCar < nbCarMax){
			Car* newCar;
			int road;
			//We wait for a random time
			int delay = rand()%((REACH_MAX+1) - REACH_MIN) + REACH_MIN;
			usleep(delay);

			//We choose a road at random
			road = rand()%2;

			//We created the car on this road
			newCar = start_car(nbCar+1, road, intersection, pidSon + nbCar);
			if(newCar != NULL){
				//And we bring it into the intersection
				enterintersection(newCar);
				return NULL;
			}
			nbCar++;
		}
	}
	//If in manual mode
	else{
		char* buf = (char*) malloc(sizeof(char));
		//As long as we have not exceeded the number of cars required
		do{
			*buf = '\0';
			//We read the standard entry
			read(0, buf, 1);
			//if we press a
			if(*buf == 'a'){
				Car* newCar;
				//We created the car on the road 0
				newCar = start_car(nbCar+1, 0, intersection, pidSon + nbCar);
				if(newCar != NULL){
					//And we bring it into the intersection
					enterintersection(newCar);
					return NULL;
				}
				nbCar++;
			}
			//if we press z
			else if(*buf == 'z'){
				Car* newCar;
				//We created the car on road 1
				newCar = start_car(nbCar+1, 1, intersection, pidSon + nbCar);
				if(newCar != NULL){
					//And we bring it into the intersection
					enterintersection(newCar);
					return NULL;
				}
				nbCar++;
			}
		}while(nbCar < nbCarMax && *buf != '\0');
	}

	return pidSon;
}
