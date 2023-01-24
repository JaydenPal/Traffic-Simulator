#include "trafficSimulator.h"
#include "event.h"
#include "road.h"

/* Function declarations: */
TrafficData* readTrafficDataFromInputFile( char* name );

/* printNames
 * input: none
 * output: none
 *
 * Prints names of the students who worked on this solution
 */
void printNames( )
{
    /* TODO : Fill in you and your partner's names (or N/A if you worked individually) */
    printf("This solution was completed by:\n");
    printf("Jayden Palacios\n");
    printf("\"N/A\")>\n");
}


/* trafficSimulator
 * input: char* name of file to read
 * output: N/A
 *
 * Read in the traffic data from the file whose name is stored in char* filename
 * Simulate the rules outlined in the project 2 description
 */
void trafficSimulator( char* filename ){
    /* TODO: complete this function */
    int i, curStep = 0, max = 0, totalCars = 0, lastCarMove = 0;
    double avg = 0; 
    bool gridlock = false;
    /* Read in the traffic data from filename.  It's recommended you call readTrafficDataFromInputFile to do this: */
    TrafficData* tData = readTrafficDataFromInputFile( filename );
    /* Loop until all events processed and either all cars reached destination or gridlock has occurred */
    do{
        /* Update the state of every traffic light */
         for(i = 0; i < tData->numOfRoads; i++){
             updateLight(curStep, tData->roadArr[i]);
         }
        /* Loop on events associated with this time step */
        while(getNextPQ(tData->eventQueue) != NULL && getNextPQ(tData->eventQueue)->stepToAdd == curStep ){
            Event* curEvent = dequeuePQ(tData->eventQueue);
            if(curEvent->type == ADD_CAR_EVENT){
                printf("CYCLE %d - ADD_CAR_EVENT - Cars enqueued on road from %d to %d\n", curStep, curEvent->from, curEvent->to);
                tData->numOfCars += curEvent->numOfCars;
                totalCars += curEvent->numOfCars;
                runEvent(tData->roadArr, tData->numOfRoads, curEvent);
            } else {
                printf("\nCYCLE %d - PRINT_ROADS_EVENT - Current contents of the roads:\n", curStep);
                for(i = 0; i < tData->numOfRoads; i++){
                    printRoad(tData->roadArr[i]);
                }
                free(curEvent);
                printf("\n");
            }
        }
        /* First try to move cars through the next intersection */
        
        for(i = 0; i < tData->numOfRoads; i++){
            attemptMoveFirstCar(tData, tData->roadArr[i], curStep, &max, &avg, &lastCarMove);
         }
        // /* Second move waiting cars onto the end of each road if possible */
        for(i = 0; i < tData->numOfRoads; i++){
            moveCarFromQueue(tData->roadArr[i], curStep, &lastCarMove);
        }
        /* Third move cars forward on every road (only those that haven't moved yet this time step) */
        for(i = 0; i < tData->numOfRoads; i++){
            attemptCarMoves(tData->roadArr[i], curStep, &lastCarMove);
        }
        curStep++;
        if(curStep - lastCarMove > tData->longestTime && isEmptyPQ(tData->eventQueue)){
            gridlock = true;
            printf("CYCLE %d - Gridlock detected.\n", curStep);
            break;
        }
    }while((tData->numOfCars) > 0 || !isEmptyPQ(tData->eventQueue));
    if(gridlock == false){
        printf("\nAverage number of time steps to the reach their destination is %.2lf.\n", avg / (double)totalCars);
        printf("Maximum number of time steps to the reach their destination is %d.\n", max);
    }
    freeTrafficData(tData);
}

/* readTrafficDataFromInputFile
 * input: char* filename of file to read
 * output: TrafficData* which stores the data of this road network
 *
 * Create a new TrafficData variable and read in the traffic data from the file whose name is stored in char* filename
 */
TrafficData* readTrafficDataFromInputFile( char* filename ){
    int i, j, roadCount = 0, numOfPrints;
    /* open the file */
    FILE *pFile = fopen( filename, "r" );
    TrafficData* ret = (TrafficData*)malloc(sizeof(TrafficData)* 1);
    ret->eventQueue = createPQ();
    ret->longestTime = 0;
    int numOfVertex;
    int numEdges;

    fscanf(pFile, "%d", &numOfVertex);
    fscanf(pFile, "%d", &numEdges);
    
    ret->roadArr = (RoadData**)malloc(sizeof(RoadData*)*numEdges);
    ret->numOfRoads = numEdges;
    ret->graph = createGraph(numOfVertex);

    fseek(pFile, 3, SEEK_CUR);
    for(i = 0; i < numOfVertex; i++){
        int numOfIncoming;
        fscanf(pFile, "%d", &numOfIncoming);
        fseek(pFile, 1, SEEK_CUR);
        for(j = 0; j < numOfIncoming; j++){
            if(numOfIncoming == 0){
                break;
            }
            int on, off, reset,roadLen,from;
            fscanf(pFile, "%d", &from);
            fscanf(pFile, "%d", &roadLen);
            fscanf(pFile, "%d", &on);
            fscanf(pFile, "%d", &off);
            fscanf(pFile, "%d", &reset);
            setEdge(ret->graph, from, i, roadLen);
            if(reset > ret->longestTime){
                ret->longestTime = reset;
            }
            int cycle[3] = {on, off, reset};
            RoadData* road = createRoad(roadLen, i, from, cycle);
            ret->roadArr[roadCount] = road;
            roadCount++;
        }
        fseek(pFile, 2, SEEK_CUR);
    }
    int numOfEvents;
    fscanf(pFile, "%d", &numOfEvents);
    ret->numOfEvents = numOfEvents;
    fseek(pFile, 3, SEEK_CUR);
    for(i = 0; i < numOfEvents; i++){
        int from, to, stepToPerform, numOfCars;
        Event* event = (Event*)malloc(sizeof(Event)*1);
        event->carQ = createQueue();
        fscanf(pFile, "%d", &from);
        fscanf(pFile, "%d", &to);
        fscanf(pFile, "%d", &stepToPerform);
        fseek(pFile, 1, SEEK_CUR);
        fscanf(pFile, "%d", &numOfCars);
        fseek(pFile, 1, SEEK_CUR);
        event->numOfCars = numOfCars;
        event->from = from;
        event->to = to;
        event->stepToAdd = stepToPerform;
        for(j = 0; j < numOfCars; j++){
            int carDest;
            fscanf(pFile, "%d", &carDest);
            Car* car = createCar();
            car->curRoad = from;
            car->from = from;
            car->destination = carDest;
            car->stepAdded = stepToPerform;
            car->stepAddedToRoad = 0;
            enqueue(event->carQ, car);
        }
        event->type = ADD_CAR_EVENT;
        enqueueByPriority(ret->eventQueue, event, stepToPerform);
    }
    fseek(pFile, 3, SEEK_CUR);
    fscanf(pFile, "%d", &numOfPrints);
    fseek(pFile, 2, SEEK_CUR);
    ret->stepsToPrint = (int*)malloc(sizeof(int)*numOfPrints);
    for(i = 0; i < numOfPrints; i++){
        Event* event = (Event*)malloc(sizeof(Event)*1);
        fscanf(pFile, "%d", &ret->stepsToPrint[i]);
        event->stepToAdd = ret->stepsToPrint[i];
        event->type = PRINT_ROADS_EVENT;
        enqueueByPriority(ret->eventQueue, event, event->stepToAdd);
    }
    ret->numOfCars = 0;
    /* close file */
    fclose( pFile );
    return ret; 
}

void attemptMoveFirstCar(TrafficData* tData, RoadData* road, int curStep, int *max, double *avg, int* lastCarMove){
    Car* firstCar = road->cars[0];
    if(firstCar == NULL){
        return;
    }
    if(firstCar->destination == road->to && road->curLight != RED){
        firstCar->stepRemoved = curStep;
        tData->numOfCars--;
        int temp = (firstCar->stepRemoved - firstCar->stepAdded) + 1;
        printf("CYCLE %d - Car successfully traveled from %d to %d in %d time steps.\n", curStep, firstCar->from, firstCar->destination, temp);
        if((firstCar->stepRemoved - firstCar->stepAdded) + 1 > *max){
            *max = temp;
        }
        *avg += temp;
        road->cars[0] = NULL;
        *lastCarMove = curStep;
        free(firstCar);
        return;
    }
    int nextOnShortestPath, i;
    
    getNextOnShortestPath(tData->graph, road->to, firstCar->destination, &nextOnShortestPath);

    for(i = 0; i < tData->numOfRoads; i++){
        if(tData->roadArr[i]->to == nextOnShortestPath && tData->roadArr[i]->from == road->to){
            RoadData* nextRoad = tData->roadArr[i];
            if(nextRoad->cars[nextRoad->len - 1] == NULL && road->curLight != RED){
                firstCar->stepAddedToRoad = curStep;
                nextRoad->cars[nextRoad->len - 1] = firstCar;
                road->cars[0] = NULL;
                *lastCarMove = curStep;
            }
            break;
        }
    }
}

void runEvent(RoadData** roadArr, int numOfRoads, Event* event){
    int i;
    for(i = 0; i < numOfRoads; i++){
        if(roadArr[i]->to == event->to && roadArr[i]->from == event->from){
            mergeQueues(roadArr[i]->carQ, event->carQ);
        }
    }
    freeQueue(event->carQ);
    free(event);
}

void freeTrafficData(TrafficData* tData){
    int i;
    freeGraph(tData->graph);
    for(i = 0; i < tData->numOfRoads; i++){
        freeRoad(tData->roadArr[i]);
    }
    free(tData->roadArr);
    freePQ(tData->eventQueue);
    free(tData->stepsToPrint);
    free(tData);
}