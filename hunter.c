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
    HNodeType* newNode = (HNodeType *)malloc(sizeof(HNodeType));
    newNode->Hunter = hunter;
    newNode->nextNode = NULL;

    if (room->hunters->head == NULL) {
        room->hunters->head = newNode;
    } else {
        room->hunters->tail->nextNode = newNode;
    }
    room->hunters->tail = newNode;
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
    if (room->hunters == NULL) {
        return C_FALSE;
    } else {
        return C_TRUE;
    }
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
    //lock to avoid conflicts
    sem_wait(&room->Mutex);

    if (room->evidences->head == NULL) {
        //unlock to allow others to act
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
    //unlock to allow others to act
    sem_post(&room->Mutex);
}

/*
Function - moveHunter
Purpose - moves a hunter to an adjacent room
in/out - hunter (HunterType *) 
*/
void moveHunter(HunterType *hunter) {
    int adjacentRooms = 0;
    RNodeType *currNode = hunter->room->rooms->head;
    while (currNode != NULL) {
        adjacentRooms++;
        currNode = currNode->nextNode;
    }
    int x  = randInt(0, adjacentRooms);
    currNode = hunter->room->rooms->head;
    for (int i = 0; i < x; i++) {
        currNode = currNode->nextNode;
    }
    RoomType *newRoom = currNode->currRoomObj;

    //lock to avoid conflicts
    sem_wait(&hunter->room->Mutex);
    sem_wait(&newRoom->Mutex);

    setHunterRoom(hunter, newRoom);
    l_hunterMove(hunter->name, hunter->room->name);

    //unlock to allow others to act
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

    while ((hunter->boredemTimer < BOREDOM_MAX) && (hunter->fear < FEAR_MAX)) {
        //lock to avoid conflicts
        sem_wait(&hunter->room->Mutex);
        int ghostInRoom = ghostPresent(hunter->room);

        if (ghostInRoom == C_TRUE) {
            hunter->fear++;
            hunter->boredemTimer = 0;
            int action = randInt(0, 3);

            if (action == 0) {
                hunterCollect(hunter, hunter->room);
            } else if (action == 1) {
                moveHunter(hunter);
            } else {
                if (reviewHunterEvidence(hunter) == C_TRUE) {
                    //unlock to allow others to act
                    sem_post(&hunter->room->Mutex);
                    l_hunterExit(hunter->name, LOG_EVIDENCE);
                    pthread_exit(NULL);
                }
            }

        } else {
            hunter->boredemTimer++;
            int action = randInt(0, 3);

            if (action == 0) {
                hunterCollect(hunter, hunter->room);
            } else if (action == 1) {
                moveHunter(hunter);
            } else {
                if (reviewHunterEvidence(hunter) == C_TRUE) {
                    //unlock to allow others to act
                    sem_post(&hunter->room->Mutex);
                    l_hunterExit(hunter->name, LOG_EVIDENCE);
                    pthread_exit(NULL);
                }
            }
        }

        if (hunter->fear >= FEAR_MAX) {
            l_hunterExit(hunter->name, LOG_FEAR);
            //unlock to allow others to act
            sem_post(&hunter->room->Mutex);
            pthread_exit(NULL);
        } else if (hunter->boredemTimer >= BOREDOM_MAX) {
            l_hunterExit(hunter->name, LOG_BORED);
            //unlock to allow others to act
            sem_post(&hunter->room->Mutex);
            pthread_exit(NULL);
        }
        //unlock to allow others to act
        sem_post(&hunter->room->Mutex);
    }

    pthread_exit(NULL);
}

