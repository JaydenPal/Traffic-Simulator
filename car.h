#ifndef _car_h
#define _car_h

typedef struct Car
{
    //TODO:  Add information to store about cars
    int stepAdded;
    int stepRemoved;
    int from;
    int stepAddedToRoad;
    int stepEnqueued;
    int destination;
    int curRoad;
}  Car;
Car* createCar();
#endif
