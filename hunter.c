#include "defs.h"
/*
Function - initEvidenceList
Purpose - initializes a Hunter list
out - list (HunterListType *) 
*/
void initHunterList(HunterListType *list){
    list->head = NULL;
    list->tail = NULL;
}

/*
Function - initHunter
Purpose - initializes a Hunter 
in - name (char *), equipment (EvidenceType *), start (RoomType *), sharedEvidenceList (EvidenceListType *), house (HouseType *)
out - hunter (HunterType *)
return (HunterType *)
*/
HunterType* initHunter(char *name, EvidenceType *equipment, RoomType *start, EvidenceListType *sharedEvidenceList, HouseType *house) {
    HunterType *hunter = (HunterType*)malloc(sizeof(HunterType));

    strncpy(hunter->name, name, MAX_STR - 1);
    hunter->name[MAX_STR - 1] = '\0';
    hunter->equipment = *equipment;
    hunter->room = start;
    hunter->boredemTimer = 0;
    hunter->fear = 0;
    hunter->evidenceList = sharedEvidenceList;
    hunter->house = house;
    
    l_hunterInit(name, *equipment);
    return hunter;
}

/*
Function - addHunter
Purpose - adds a hunter to the house
in - h (HouseType *)
out - hunter (HunterType*)
*/
void addHunter(HunterType *hunter, HouseType *h) {
    if (h->hunters.head == NULL) {
        h->hunters.head = (HNodeType *)malloc(sizeof(HNodeType));
        h->hunters.head->Hunter = hunter;
        h->hunters.head->nextNode = NULL;
        h->hunters.tail = h->hunters.head;
    }
    else {
        HNodeType* newNode = (HNodeType *)malloc(sizeof(HNodeType));
        newNode->Hunter = hunter;
        newNode->nextNode = NULL;

        h->hunters.tail->nextNode = newNode;
        newNode->nextNode = NULL;
        h->hunters.tail = newNode;
    }
}

/*
Function - setHunterRoom
Purpose - allocates space for a hunter room
in - room (RoomType *)
out - hunter (HunterType *)
*/
void setHunterRoom(HunterType *hunter, RoomType *room) {
    // Log the move for debugging
    printf("[HUNTER MOVE] Setting %s's room to [%s]\n", hunter->name, room->name);

    // Update the current room of the hunter
    RoomType *oldRoom = hunter->room; // Save the current room

    // Remove the hunter from the old room's hunter list
    if (oldRoom->hunters != NULL) {
        HNodeType *currNode = oldRoom->hunters->head;
        HNodeType *prevNode = NULL;
        while (currNode != NULL) {
            if (currNode->Hunter == hunter) {
                // Found the hunter, now remove from old room
                if (prevNode == NULL) {
                    // Removing the head of the list
                    oldRoom->hunters->head = currNode->nextNode;
                } else {
                    // Bypass the current node
                    prevNode->nextNode = currNode->nextNode;
                }

                if (currNode->nextNode == NULL) {
                    // Updating the tail if we are removing the last element
                    oldRoom->hunters->tail = prevNode;
                }
                free(currNode); // Free the node
                break; // Exit after removing the hunter
            }
            prevNode = currNode;
            currNode = currNode->nextNode;
        }
    }

    // Now add the hunter to the new room
    HNodeType *newNode = (HNodeType *)malloc(sizeof(HNodeType));
    newNode->Hunter = hunter;
    newNode->nextNode = NULL;

    if (room->hunters->head == NULL) {
        room->hunters->head = newNode;
    } else {
        room->hunters->tail->nextNode = newNode;
    }
    room->hunters->tail = newNode;

    // Finally, update the hunter's room
    hunter->room = room; // Update the current room of the hunter
    printf("[HUNTER MOVE] %s has moved into [%s]\n", hunter->name, room->name);
}


/*
Function - cleanupHunterData
Purpose - frees hunter(struct) memory
in/out - hunter (HunterType*)
*/
void cleanupHunterData(HunterType *hunter) {
    if (hunter == NULL) {
        return;
    }
    if (hunter->evidenceList != NULL) {
        cleanupEvidenceList(hunter->evidenceList);
        hunter->evidenceList = NULL;
    }
    free(hunter);
}

/*
Function - cleanupHunterList
Purpose - frees a hunter list
in/out - list (HunterListType *) 
*/
void cleanupHunterList(HunterListType *list) {
    //begin at the head
    HNodeType *currNode = list->head;
    //traverse list until last node is found
    while (currNode != NULL) {
        HNodeType *tempNode = currNode;
        currNode = currNode->nextNode;
        if (tempNode->Hunter != NULL) {
            cleanupHunterData(tempNode->Hunter);
        }
        free(tempNode);
    }
    //set list to NULL
    list->head = NULL;
    list->tail = NULL;
}

/*
Function - hunterPresent
Purpose - checks to see if a hunter is in a given room
in - room (RoomType *) 
*/
int hunterPresent(RoomType *room) {
    if (room == NULL) {
        printf("Room is NULL\n");
        return C_FALSE; // Indicate no hunters if the room is NULL
    }

    // Check if the hunters list in the room is NULL
    if (room->hunters == NULL || room->hunters->head == NULL) {
        return C_FALSE; // No hunters present in the room
    }

    // There are hunters present in the room
    return C_TRUE;
}

/*
Function - addEvidenceToSharedHouse
Purpose - adds a piece of evidence to the house evidence shared collection
in - hunterEvidence (EvidenceType)
out - houseEvidence (EvidenceListType *)
*/
void addEvidenceToSharedHouse(EvidenceListType *houseEvidence, EvidenceType hunterEvidence){
    ENodeType *newNode = (ENodeType*)malloc(sizeof(ENodeType));
    newNode->evidenceObj = (EvidenceType*)malloc(sizeof(EvidenceType));

    *(newNode->evidenceObj) = hunterEvidence;
    newNode->nextNode = NULL;

    if (houseEvidence->head == NULL) {
        houseEvidence->head = newNode;
    } else {
        houseEvidence->tail->nextNode = newNode;
    } 
    houseEvidence->tail = newNode;
}

/*
Function - addEvidenceToSharedHouse
Purpose - adds a piece of evidence to the house shared collection
in - list (EvidenceListType *), evidence (EvidenceType)
return - int C_TRUE/C_FALSE
*/
int evidenceExists(EvidenceListType *list, EvidenceType evidence) {
    ENodeType *currNode = list->head;
    while (currNode != NULL) {
        if (*(currNode->evidenceObj) == evidence) {
            return C_FALSE;
        }
        currNode = currNode->nextNode;
    }
    return C_TRUE;
}

/*
Function - hunterCollect
Purpose - adds evidence to the house evidence list (rejects duplicates)
in/out - hunter (HunterType *), room (RoomType *) 
*/
void hunterCollect(HunterType *hunter, RoomType *room) {
    // Lock the room mutex
    sem_wait(&room->Mutex);

    if (room->evidences->head == NULL) {
        // Unlock before exiting
        sem_post(&room->Mutex);
        return;
    }
    ENodeType *currNode = room->evidences->head;
    while (currNode != NULL) {
        if (*(currNode->evidenceObj) == hunter->equipment) {
            if (evidenceExists(hunter->evidenceList, *(currNode->evidenceObj)) == C_TRUE) {
                addEvidenceToSharedHouse(hunter->evidenceList, hunter->equipment);
                l_hunterCollect(hunter->name, *(currNode->evidenceObj), room->name);
            }
        }
        currNode = currNode->nextNode;
    }
    // Unlock the room mutex after processing evidence
    sem_post(&room->Mutex);
}

/*
Function - moveHunter
Purpose - moves a hunter to an adjacent room
in/out - hunter (HunterType *) 
*/
void moveHunter(HunterType *hunter) {
    // Count the number of adjacent rooms
    int adjacentRooms = 0;
    RNodeType *currNode = hunter->room->rooms->head;
    while (currNode != NULL) {
        adjacentRooms++;
        currNode = currNode->nextNode;
    }

    // Ensure there are adjacent rooms to move to
    if (adjacentRooms == 0) {
        printf("[HUNTER MOVE] No adjacent rooms to move to for %s\n", hunter->name);
        return;
    }

    // Choose a random room index
    int x = randInt(0, adjacentRooms - 1); // Ensure x is within bounds
    printf("[HUNTER MOVE] Hunter %s has %d adjacent rooms, moving to index %d\n", hunter->name, adjacentRooms, x);

    // Reset currNode to the head of the list to select the room
    currNode = hunter->room->rooms->head;
    for (int i = 0; i < x; i++) {
        currNode = currNode->nextNode; // Iterate to the selected room
    }
   
    RoomType *newRoom = currNode->currRoomObj;

    // Lock the current and new room mutexes
    sem_wait(&hunter->room->Mutex);
    sem_wait(&newRoom->Mutex);

    // Log the current room before moving
    printf("[HUNTER MOVE] %s moving from %s to %s\n", hunter->name, hunter->room->name, newRoom->name);
   
    // Update the hunter's room
    setHunterRoom(hunter, newRoom);

    // Log the new room assignment
    printf("[HUNTER MOVE] %s has moved into [%s]\n", hunter->name, newRoom->name);

    // Unlock both room mutexes
    sem_post(&hunter->room->Mutex);
    sem_post(&newRoom->Mutex);
}

/*
Function - reviewHunterEvidence
Purpose - all hunters review 
in - hunter (HunterType *) 
return -  int C_TRUE/C_FALSE
*/
int reviewHunterEvidence(HunterType *hunter) {
    //lock to avoid conflicts
    sem_wait(&hunter->evidenceList->Mutex);

    int numOfEvidences = 0;
    ENodeType *currNode = hunter->evidenceList->head;
    while (currNode != NULL) {
        numOfEvidences++;
        currNode = currNode->nextNode;
    }

    //unlock to allow others to act
    sem_post(&hunter->evidenceList->Mutex);

    if (numOfEvidences >= 3) {
        l_hunterReview(hunter->name, LOG_SUFFICIENT);
        return C_TRUE;
    } else {
        l_hunterReview(hunter->name, LOG_INSUFFICIENT);
        return C_FALSE;
    }
}

/*
Function - *threadHunter 
Purpose - starts the hunter thread
in/out - hArgs (Void* )  
*/
void *threadHunter(void *hArgs) {
    HunterType *hunter = (HunterType *)hArgs;

    printf("Hunter %s entered threadHunter, starting in room %s\n", hunter->name, hunter->room->name);

    while (hunter->boredemTimer < BOREDOM_MAX && hunter->fear < FEAR_MAX) {
        // Lock the room mutex
        sem_wait(&hunter->room->Mutex);
       
        int ghostInRoom = ghostPresent(hunter->room); // Check ghost presence
        printf("Hunter %s checking room %s: Ghost in room? %d\n", hunter->name, hunter->room->name, ghostInRoom);

        if (ghostInRoom == C_TRUE) {
            hunter->fear++; // Increment fear level
            hunter->boredemTimer = 0; // Reset boredom timer
            printf("Hunter %s encountered ghost! Fear: %d, Boredom reset\n", hunter->name, hunter->fear);
           
            // Randomly choose an action
            int action = randInt(0, 3);
            printf("Hunter %s chose action %d with ghost present\n", hunter->name, action);
           
            // Actions based on the presence of a ghost
            switch (action) {
                case 0:
                    hunterCollect(hunter, hunter->room); // Attempt to collect evidence
                    break;
                case 1:
                    moveHunter(hunter); // Move to another room
                    break;
                case 2:
                    // Review evidence and possibly exit if enough evidence is collected
                    if (reviewHunterEvidence(hunter) == C_TRUE) {
                        printf("Hunter %s collected enough evidence and exits\n", hunter->name);
                        sem_post(&hunter->room->Mutex); // Unlock before exiting
                        l_hunterExit(hunter->name, LOG_EVIDENCE);
                        pthread_exit(NULL); // Exit the thread
                    }
                    break;
            }
        } else {
            // If no ghost is present
            hunter->boredemTimer++; // Increment boredom timer
            printf("Hunter %s boredom incremented: %d\n", hunter->name, hunter->boredemTimer);

            // Randomly choose an action
            int action = randInt(0, 3);
            printf("Hunter %s chose action %d without ghost present\n", hunter->name, action);

            switch (action) {
                case 0:
                    hunterCollect(hunter, hunter->room); // Attempt to collect evidence
                    break;
                case 1:
                    moveHunter(hunter); // Move to another room
                    break;
                case 2:
                    // Review evidence and possibly exit if enough evidence is collected
                    if (reviewHunterEvidence(hunter) == C_TRUE) {
                        printf("Hunter %s collected enough evidence and exits\n", hunter->name);
                        sem_post(&hunter->room->Mutex); // Unlock before exiting
                        l_hunterExit(hunter->name, LOG_EVIDENCE);
                        pthread_exit(NULL); // Exit the thread
                    }
                    break;
            }
        }

        // Check for exit conditions based on fear or boredom
        if (hunter->fear >= FEAR_MAX) {
            printf("Hunter %s exits due to fear\n", hunter->name);
            l_hunterExit(hunter->name, LOG_FEAR);
            sem_post(&hunter->room->Mutex); // Unlock before exiting
            pthread_exit(NULL); // Exit the thread
        } else if (hunter->boredemTimer >= BOREDOM_MAX) {
            printf("Hunter %s exits due to boredom\n", hunter->name);
            l_hunterExit(hunter->name, LOG_BORED);
            sem_post(&hunter->room->Mutex); // Unlock before exiting
            pthread_exit(NULL); // Exit the thread
        }

        sem_post(&hunter->room->Mutex); // Unlock the room mutex after finishing the logic
    }

    printf("Hunter %s exiting thread naturally\n", hunter->name);
    pthread_exit(NULL); // Exit the thread
}