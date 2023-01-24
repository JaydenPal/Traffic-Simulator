#ifndef _trafficSimulator_h
#define _trafficSimulator_h
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "graph.h"
#include "queue.h"
#include "priorityQueue.h"

typedef struct TrafficData
{
    //Optional TODO - Fill this struct in with the data needed to simulate traffic.  Suggested data:
    /* graph representing the road network */
    Graph *graph;
    /* array of the roads of the graph in the sequence they were added to the graph */
    RoadData **roadArr;
    int numOfRoads;
    /* priority queue of events where the priority represents the time the event will occur */
    PriorityQueue *eventQueue;
    int numOfEvents;
    /* track the number of cars still in the simulator */
    int numOfCars;
    /* track the longest number of time steps that any light takes to cycle around.  This is useful for detecting gridlock. */
    int longestTime;
    int* stepsToPrint;
}  TrafficData;


void printNames( );
void attemptMoveFirstCar(TrafficData* tData, RoadData* road, int curStep, int *max, double *avg, int* lastCarMove);
void trafficSimulator( char* filename );
void runEvent(RoadData** roadArr, int numOfRoads, Event* event);
void freeTrafficData(TrafficData* tData);
#endif
