#include "trafficSimulator.h"
#include "road.h"
#include "graph.h"
/* Suggested TODO - Write a function to create road.
 */
RoadData* createRoad(int roadLen, int to, int from, int cycle[]){
    RoadData * ret = (RoadData*)malloc(sizeof(RoadData));
    ret->cars = (Car**)malloc(sizeof(Car*)*roadLen);
    int i;
    for(i = 0; i < roadLen; i++){
        ret->cars[i] = NULL;
    }
    ret->to = to;
    ret->carQ = createQueue();
    ret->from = from;
    ret->len = roadLen;
    ret->greenOn = cycle[0];
    ret->greenOff = cycle[1];
    ret->cycleReset = cycle[2];
    return ret;
}

/* Suggested TODO - Write a function to free a road.
 */
void freeRoad(RoadData* road){    
    freeQueue(road->carQ);
    int i;
    for(i = 0; i < road->len; i++){
        if(road->cars[i] != NULL){
            free(road->cars[i]);
        }
    }
    free(road->cars); /*may need to free data inside but im thinking i will free car pointers individually*/
    free(road);
}

/* Suggested TODO - Write a function that attempts to move the cars on a given road forward (if space ahead of them was empty).
 */
void attemptCarMoves(RoadData* road, int curStep, int* lastCarMove){
    int i;
    for(i = road->len-1; i > 0; i--){     /* i > 0 to avoid moving first car*/
        if(road->cars[i] == NULL || (road->cars[i] != NULL && (road->cars[i]->stepEnqueued == curStep || road->cars[i]->stepAddedToRoad == curStep))){
            continue;
        }
        if(road->cars[i -1] == NULL){
            road->cars[i - 1] = road->cars[i];
            road->cars[i] = NULL;
            *lastCarMove = curStep;
            i--; /*move foward two to avoid moving car just moved*/
        }
    }
}

/* Suggested TODO - Write a function to attempt to move the car at the first position on a given road
 * to the last position of a another given road.
 */


/* Suggested TODO - Write a function to attempt to move a car from the add car queue onto the end of this road.
 */
void moveCarFromQueue(RoadData* road, int curStep, int* lastCarMove){
    if(road->cars[road->len - 1] != NULL || getNext(road->carQ) == NULL){
        return;
    }
    Car* car = dequeue(road->carQ);
    car->stepEnqueued = curStep;
    road->cars[road->len - 1] = car;
    *lastCarMove = curStep;
}
/* Suggested TODO - Write a function to update the current state of the traffic light based on the current cycle.
 */

void updateLight(int curStep, RoadData* road){
    
    if(curStep % road->cycleReset < road->greenOff && curStep % road->cycleReset >= road->greenOn){
        road->curLight = GREEN;
    } else {
        road->curLight = RED;
    }
    
}

/* Suggested TODO - Write a function to print the destinations of the cars on the road
 */

void printRoad(RoadData* road){
    int i;
    printf("Cars on the road from %d to %d:\n", road->from, road->to);
    for(i = 0; i < road->len; i++){
        if(road->cars[i] != NULL){
            printf("%d ", road->cars[i]->destination);
        } else if(road->cars[i] == NULL){
            printf("- ");
        }
    }
    if(road->curLight == GREEN){
        printf("(GREEN light)\n");
    } else {
        printf("(RED light)\n");
    }
}

