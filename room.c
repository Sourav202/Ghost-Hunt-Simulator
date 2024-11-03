#include "defs.h"

/*
Function - initRoomList
Purpose - initializes a room list
out - list (RoomListType *)
*/
void initRoomList(RoomListType *list) {
    list->head = NULL;
    list->tail = NULL;
}

/*
Function - createRoom
Purpose - initializes a room 
in - name (char *)
*/
RoomType* createRoom(char *name) {
    //init room
    //printf("created");
    RoomType *room = (RoomType*)malloc(sizeof(RoomType));
    strncpy(room->name, name, MAX_STR - 1);
    room->name[MAX_STR - 1] = '\0';

    //init roomList, evidenceList, and hunterList
    room->rooms = (RoomListType*)malloc(sizeof(RoomListType));
    room->rooms->head = NULL;
    room->rooms->tail = NULL;

    room->evidences = (EvidenceListType*)malloc(sizeof(EvidenceListType));
    initEvidenceList(room->evidences);

    room->hunters = (HunterListType*)malloc(sizeof(HunterListType)); 
    room->hunters->head = NULL;
    room->hunters->tail = NULL;

    sem_init(&room->Mutex, 0, 1);
    return room;
}

/*
Function - connectRooms
Purpose - Creates a two-way connection between two given rooms
in/out - r1, r2 (RoomType *)
*/
void connectRooms(RoomType *r1, RoomType *r2) {
    //printf("before RnodeType");
    RNodeType *room1 = (RNodeType*)malloc(sizeof(RNodeType));
    RNodeType *room2 = (RNodeType*)malloc(sizeof(RNodeType));
    //printf("after RnodeType");
    room1->prevNodeRoom = NULL;
    room1->currRoomObj = r2;
    room1->nextNode = r1->rooms->head;

    if (r1->rooms->head != NULL) {
        r1->rooms->head->prevNodeRoom = room1;
    }
    r1->rooms->head = room1;

    if (r1->rooms->tail == NULL) {
        r1->rooms->tail = room1;
    }

    room2->prevNodeRoom = NULL;
    room2->currRoomObj = r1;
    room2->nextNode = r2->rooms->head;

    if (r2->rooms->head != NULL) {
        r2->rooms->head->prevNodeRoom = room2;
    }
    r2->rooms->head = room2;

    if (r2->rooms->tail == NULL) {
        r2->rooms->tail = room2;
    }
}

/*
Function - addRoom
Purpose - Adds a room a room list
in/out - list (RoomListType *)
out - r (RoomType*)
*/
void addRoom(RoomListType *arr, RoomType *r) {
    if (arr->head == NULL) {
        arr->head = (RNodeType *)malloc(sizeof(RNodeType));
        arr->head->currRoomObj = r;
        arr->head->prevNodeRoom = NULL;
        arr->head->nextNode = NULL;
        arr->tail = arr->head;
    }
    else {
        RNodeType* newNode = (RNodeType *)malloc(sizeof(RNodeType));
        newNode->currRoomObj = r;
        newNode->nextNode = NULL;

        arr->tail->nextNode = newNode;
        newNode->prevNodeRoom = arr->tail;
        arr->tail = newNode;
    }
}

/*
Function - cleanupRoomData
Purpose - frees room(struct) memory
out - room (RoomType *)
*/
void cleanupRoomData(RoomType *room) {
    if (room == NULL) {
        return;
    }
    if (room->evidences != NULL) {
        cleanupEvidenceList(room->evidences);
        free(room->evidences);
        room->evidences = NULL;
    }
    if (room->hunters != NULL) {
        cleanupHunterList(room->hunters);
        free(room->hunters);
        room->hunters = NULL;
    }
    if (room->rooms != NULL) {
        cleanupRoomList(room->rooms);
        free(room->rooms);
    }
    sem_destroy(&room->Mutex);
    free(room);
}

/*
Function - cleanupRoomList
Purpose - frees a room list
out - list (RoomListType *)
*/
void cleanupRoomList(RoomListType *list) {
    //begin at the head
    RNodeType *currNode = list->head;
    //traverse list until last node is found
    while (currNode != NULL) {
        RNodeType *tempNode = currNode;
        currNode = currNode->nextNode;
        free(tempNode);
    }
    //set list to NULL
    list->head = NULL;
    list->tail = NULL;
}