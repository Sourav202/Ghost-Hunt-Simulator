#include "defs.h"
/*
Function - initEvidenceList
Purpose - initializes a Evidence list
out - list (EvidenceListType *)
*/
void initEvidenceList(EvidenceListType *list){
    list->head = NULL;
    list->tail = NULL;

    //init semaphore
    sem_init(&list->Mutex, 0, 1);
}

/*
Function - cleanupEvidenceData
Purpose - Frees sharedEvidenceList
in/out - list (EvidenceListType *)
*/ 
void cleanupEvidenceData(EvidenceListType *list) {
    if (list == NULL) {
        return;
    }
    //begin at the head
    ENodeType *oldEvidence;
    //traverse list until last node is found
    while ((oldEvidence = list->head) != NULL) {
        list->head = list->head->nextNode;
        free(oldEvidence);
    }
    //set list to NULL
    list->tail = NULL;
}

/*
Function - cleanupEvidenceList
Purpose - frees a room list
in/out - list (EvidenceListType *)
*/ 
void cleanupEvidenceList(EvidenceListType *list) {
    //lock to avoid conflicts
    sem_wait(&list->Mutex);

    //begin at the head
    ENodeType *currNode = list->head;
    //traverse list until last node is found
    while (currNode != NULL) {
        ENodeType *nodeNext = currNode->nextNode;
        if (currNode->evidenceObj != NULL) {
            free(currNode->evidenceObj);
        }
        currNode->evidenceObj = NULL;
        free(currNode);
        currNode = nodeNext;
    }
    //set list to NULL
    list->head = NULL;
    list->tail = NULL;

    //unlock to allow others to act
    sem_post(&list->Mutex);
    //destroy mutex
    sem_destroy(&list->Mutex);
}

/*
Function - addRoomEvidence
Purpose - adds a new peice of evidence to a room
in - ghostEvidence (EvidenceType)
out - roomEvidence (EvidenceListType *)
*/ 
void addRoomEvidence(EvidenceListType *roomEvidence, EvidenceType ghostEvidence) {
    /*
    ENodeType *currEvidence = roomEvidence->head;
    while (currEvidence != NULL) {
        if (*(currEvidence->evidenceObj) == ghostEvidence) {
            return C_FALSE;
        }
        currEvidence = currEvidence->nextNode;
    }*/
    //lock to avoid conflicts
    sem_wait(&roomEvidence->Mutex);

    ENodeType *newNode = (ENodeType *)malloc(sizeof(ENodeType));
    newNode->evidenceObj = (EvidenceType *)malloc(sizeof(EvidenceType));
    *(newNode->evidenceObj) = ghostEvidence;
    newNode->nextNode = NULL;

    if (roomEvidence->head == NULL) {
        roomEvidence->head = newNode;
    } else {
        roomEvidence->tail->nextNode = newNode;
    }
    roomEvidence->tail = newNode;

    //unlock to allow others to act
    sem_post(&roomEvidence->Mutex);
}