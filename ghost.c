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

    addRoomEvidence(roomEvidence, ghostEvidence[ghostDrop]);
    l_ghostEvidence(ghostEvidence[ghostDrop], ghost->room->name);
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
    ghost->room = newRoom;
    l_ghostMove(ghost->room->name);
}

/*
Function - *threadGhost 
Purpose - starts the ghost's thread 
out - gArgs (Void* )  
*/
void *threadGhost(void *gArgs) {
    GhostType *ghost = (GhostType *)gArgs;
    //HouseType *house = ghost->house;
    while (ghost->boredemTimer < BOREDOM_MAX) {
        sem_wait(&ghost->room->Mutex);
        int hunterInRoom = hunterPresent(ghost->room);
        //there is a hunter in the room
        if (hunterInRoom == C_TRUE) {
            ghost->boredemTimer = 0;
            int x  = randInt(0, 2);
            //ghost chooses to leave evidence while hunter is in room
            if (x == 0) {
                addGhostEvidence(ghost->evidence, ghost->room->evidences, ghost);
            }
            //ghost chooses to do nothing while hunter is in room
            else {}
            sem_post(&ghost->room->Mutex);
        }
    //there is no hunter in the room
        else {
            ghost->boredemTimer++;
            int x  = randInt(0, 3);
            //ghost chooses to move to an adjacent room
            if (x == 0) {
                moveGhost(ghost);
            }
            //ghost chooses to leave evidence while hunter NOT is in room
            else if (x == 1) {
                addGhostEvidence(ghost->evidence, ghost->room->evidences, ghost);
            }
            //ghost chooses to do nothing while hunter is NOT in room 
            else {}
            
            if (ghost->boredemTimer >= BOREDOM_MAX) {
                l_ghostExit(LOG_BORED);
                pthread_exit(NULL);
            }
            sem_post(&ghost->room->Mutex);
        }
    }
    pthread_exit(NULL);
}
