#include "defs.h"
/*
Function - initGhost 
Purpose - Initialize ghost
out - h (HouseType)  
in/out - ghost (GhostType) 
*/
void initGhost(HouseType *h, GhostType *ghost) {
    ghost->ghostType = randomGhost();
   
    // Initialize ghost's evidence based on ghostClass
    switch (ghost->ghostType) {
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

    // Randomly place the ghost in a room (that is NOT the Van room)
    int randRoom = randInt(0, NUM_OF_ROOMS - 1);
    RNodeType *currNode = h->rooms.head;
   
    while (currNode != NULL) {
        if (randRoom == 0) {
            ghost->room = currNode->currRoomObj; // Assign ghost to the room
            currNode->currRoomObj->ghost = ghost; // Set the ghost in the room
            printf("Ghost assigned to room: %s\n", ghost->room->name); // Debug print
            break; // Exit the loop after assigning the ghost
        }
        randRoom--;
        currNode = currNode->nextNode; // Move to the next room
    }

    ghost->boredemTimer = 0; // Initialize the boredom timer
    ghost->house = h; // Link the ghost to the house
    l_ghostInit(ghost->ghostType, ghost->room->name); // Log the ghost initialization
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
    // Check if the room is NULL and if the ghost is NULL
    if (room == NULL) {
        printf("Room is NULL\n");
        return C_FALSE; // Indicate no ghost
    }

    // Check for the ghost's presence
    if (room->ghost != NULL) {
        return C_TRUE; // Ghost is present
    } else {
        return C_FALSE; // No ghost in the room
    }
}


/*
Function - addGhostEvidence 
Purpose - add evidence to room evidence list 
in - ghost (GhostType)  
out - roomEvidence (EvidenceListType)
*/
void addGhostEvidence(EvidenceType ghostEvidence[], EvidenceListType *roomEvidence, GhostType *ghost) {
    int ghostDrop = randInt(0, 2); // Randomly select evidence type
    EvidenceType evidenceToDrop = ghostEvidence[ghostDrop]; // Get the evidence type to drop

    // Lock to avoid conflicts
    sem_wait(&ghost->room->Mutex);
    addRoomEvidence(roomEvidence, evidenceToDrop); // Attempt to add evidence to the room's evidence list

    // Log the action
    printf("Ghost [%d] dropped evidence: %d in room: %s\n",
           ghost->ghostType, evidenceToDrop, ghost->room->name);

    // Unlock to allow others to act
    l_ghostEvidence(evidenceToDrop, ghost->room->name);
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

    // Count the number of adjacent rooms
    while (currNode != NULL) {
        adjacentRooms++;
        currNode = currNode->nextNode;
    }

    // Select a random adjacent room
    int x = randInt(0, adjacentRooms - 1); // Ensure this is within bounds
    currNode = ghost->room->rooms->head;
   
    // Move to the x-th adjacent room
    for (int i = 0; i < x; i++) {
        currNode = currNode->nextNode;
    }
    RoomType *newRoom = currNode->currRoomObj;

    // Lock both rooms to avoid conflicts
    sem_wait(&ghost->room->Mutex);
    sem_wait(&newRoom->Mutex);

    // Log the ghost's movement
    printf("[GHOST MOVE] Ghost is moving from [%s] to [%s]\n", ghost->room->name, newRoom->name);
   
    // Remove ghost from old room
    ghost->room->ghost = NULL;

    // Assign the new room
    ghost->room = newRoom;
    ghost->room->ghost = ghost;

    // Log the ghost's new location
    printf("[GHOST MOVE] Ghost has moved into [%s]\n", ghost->room->name);

    // Unlock the rooms
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
        sem_wait(&ghost->room->Mutex); // Lock the room's mutex

        int hunterInRoom = hunterPresent(ghost->room); // Check if a hunter is in the room
        printf("Ghost checking room %s: Hunter in room? %d\n", ghost->room->name, hunterInRoom);

        if (hunterInRoom == C_TRUE) {
            // Ghost is in the room with a hunter
            ghost->boredemTimer = 0; // Reset boredom timer
            int action = randInt(0, 1); // Randomly decide to leave evidence or do nothing
            printf("Ghost chose action %d with hunters present\n", action);

            if (action == 0) {
                addGhostEvidence(ghost->evidence, ghost->room->evidences, ghost); // Leave evidence
            }
            // The ghost does not move in this case, just exits the loop iteration
        } else {
            // Ghost is not in the room with a hunter
            ghost->boredemTimer++; // Increment boredom timer
            int action = randInt(0, 2); // Randomly choose to move, leave evidence, or do nothing
            printf("Ghost boredom incremented to %d, chose action %d without hunters present\n", ghost->boredemTimer, action);

            if (action == 0) {
                // Move to an adjacent room
                moveGhost(ghost);
                printf("Ghost moved to room: %s\n", ghost->room->name);
            } else if (action == 1) {
                // Leave evidence
                addGhostEvidence(ghost->evidence, ghost->room->evidences, ghost);
            }
            // If action == 2, do nothing, just exit the loop iteration
        }

        sem_post(&ghost->room->Mutex); // Unlock the room's mutex

        // Check if boredom has reached max limit after actions
        if (ghost->boredemTimer >= BOREDOM_MAX) {
            printf("Ghost exits due to boredom\n");
            l_ghostExit(LOG_BORED);
            pthread_exit(NULL);
        }

        // Sleep for a brief moment to simulate time passing
        usleep(500000); // Sleep for 0.5 seconds (adjust as needed)
    }
   
    pthread_exit(NULL); // Exit the thread
}