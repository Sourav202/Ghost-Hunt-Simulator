#include "defs.h"
/*
Function - initGhost 
Purpose - Initialize ghost
out - h (HouseType)  
in/out - ghost (GhostType) 
*/
void initGhost(HouseType *h, GhostType *ghost){
    ghost->ghostType = randomGhost();
    //init ghost's evidence based on ghostClass
    switch(ghost->ghostType) {
        case BANSHEE:
            ghost->evidence[0] = EMF;
            ghost->evidence[1] = TEMPERATURE;
            ghost->evidence[2] = SOUND;
            break;
        case BULLIES:
            ghost->evidence[0] = EMF;
            ghost->evidence[1] = FINGERPRINTS;
            ghost->evidence[2] = SOUND;
            break;
        case PHANTOM:
            ghost->evidence[0] = TEMPERATURE;
            ghost->evidence[1] = FINGERPRINTS;
            ghost->evidence[2] = SOUND;
            break;
        case POLTERGEIST:
            ghost->evidence[0] = EMF;
            ghost->evidence[1] = TEMPERATURE;
            ghost->evidence[2] = FINGERPRINTS;
            break;
        default:
            ghost->evidence[0] = EV_UNKNOWN;
            break;
    }
    //randomly place it in a room (that is NOT Van room)
    int randRoom = randInt(0, NUM_OF_ROOMS - 1);
    RNodeType *currNode = h->rooms.head->nextNode;
    while (1) {
        if (randRoom == 0) {
            break;
        }
        randRoom--;
        currNode = currNode->nextNode;
    }
    ghost->room = currNode->currRoomObj;
    ghost->boredemTimer = 0;
    ghost->house = h;
    l_ghostInit(ghost->ghostType, ghost->room->name);
}

/*
Function - cleanupGhostData 
Purpose - Cleanup ghostdata  
in/out - ghost (GhostType) 
*/

void cleanupGhostData(GhostType *ghost) {
    //free structure
    free(ghost);
}


/*
Function - ghostPresent 
Purpose - checks if ghost is in room as calling function
out - room (RoomType)
return -  int C_TRUE/C_FALSE
*/
int ghostPresent(RoomType *room) {
    //checks ghost room
    if (room->ghost == NULL) {
        return C_FALSE;
    } else {
        return C_TRUE;
    }
}


/*
Function - addGhostEvidence 
Purpose - add evidence to room evidence list 
in - ghost (GhostType)  
out - roomEvidence (EvidenceListType)
*/
void addGhostEvidence(EvidenceType ghostEvidence[], EvidenceListType *roomEvidence, GhostType *ghost) {
    int ghostDrop = randInt(0, 2);

    //lock to avoid conflicts
    sem_wait(&ghost->room->Mutex);
    addRoomEvidence(roomEvidence, ghostEvidence[ghostDrop]);

    //unlock to allow others to act
    l_ghostEvidence(ghostEvidence[ghostDrop], ghost->room->name);
    sem_post(&ghost->room->Mutex);
}

/*
Function - moveGhost 
Purpose - move ghost to a different room (update pointer) 
in/out - ghost (GhostType)  
*/
void moveGhost(GhostType *ghost) {
    int adjacentRooms = 0;
    RNodeType *currNode = ghost->room->rooms->head;
    while (currNode != NULL) {
        adjacentRooms++;
        currNode = currNode->nextNode;
    }
    int x  = randInt(0, adjacentRooms);
    currNode = ghost->room->rooms->head;
    for (int i = 0; i < x; i++) {
        currNode = currNode->nextNode;
    }
    RoomType *newRoom = currNode->currRoomObj;

    //lock to avoid conflicts
    sem_wait(&ghost->room->Mutex);
    sem_wait(&newRoom->Mutex);

    ghost->room->ghost = NULL;
    ghost->room = newRoom;
    ghost->room->ghost = ghost;
    l_ghostMove(ghost->room->name);

    //unlock to allow others to act
    sem_post(&ghost->room->Mutex);
    sem_post(&newRoom->Mutex);
}

/*
Function - *threadGhost 
Purpose - starts the ghost's thread 
out - gArgs (Void* )  
*/
void *threadGhost(void *gArgs) {
    GhostType *ghost = (GhostType *)gArgs;
    printf("Starting ghost thread in room %s\n", ghost->room->name);

    while (ghost->boredemTimer < BOREDOM_MAX) {
        sem_wait(&ghost->room->Mutex);
        int hunterInRoom = hunterPresent(ghost->room);
        printf("Ghost checking room %s: Hunter in room? %d\n", ghost->room->name, hunterInRoom);

        if (hunterInRoom == C_TRUE) {
            ghost->boredemTimer = 0;
            int action = randInt(0, 1);
            printf("Ghost chose action %d with hunters present\n", action);

            if (action == 0) {
                addGhostEvidence(ghost->evidence, ghost->room->evidences, ghost);
            }
            sem_post(&ghost->room->Mutex); // release lock and proceed to the next iteration
        } else {
            ghost->boredemTimer++;
            int action = randInt(0, 2);
            printf("Ghost boredom incremented to %d, chose action %d without hunters present\n", ghost->boredemTimer, action);

            if (action == 0) {
                sem_post(&ghost->room->Mutex); // release the lock before moving
                moveGhost(ghost);
            } else if (action == 1) {
                addGhostEvidence(ghost->evidence, ghost->room->evidences, ghost);
                sem_post(&ghost->room->Mutex); // release lock after leaving evidence
            } else {
                sem_post(&ghost->room->Mutex); // release lock when doing nothing
            }
        }

        if (ghost->boredemTimer >= BOREDOM_MAX) {
            printf("Ghost exits due to boredom\n");
            l_ghostExit(LOG_BORED);
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}