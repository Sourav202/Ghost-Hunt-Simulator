#include "defs.h"

/*
Function - printRoomList 
Purpose - Prints the rooms of the house 
out - list (RoomListType*)
*/
void printRoomList(RoomListType *list) {
    if (list == NULL) {
        printf("room list is null\n");
        return;
    }

    RNodeType *curNode = list->head;
    if (curNode == NULL) {
        printf("no rooms in house\n");
        return;
    }
    
    while (curNode != NULL) {
        RoomType *room = curNode->currRoomObj;
        if (room != NULL) {
            printf("ROOM NAME: %s\n", room->name);
        }
        else {
            printf("NULL room in list found.\n");
        }
        curNode = curNode->nextNode;
    }
    return;
}

/*
Function - printHunterList 
Purpose - Prints the hunters in a given room   
out - list (RoomListType*)
*/
void printHunterList(HunterListType *list) {
    HNodeType *currNode = list->head;
    while (currNode != NULL) {
        HunterType *hunter = currNode->Hunter;
        printf("Hunter: %s |", hunter->name);
        printf(" Equipment: ");
        char equip[MAX_STR];
        evidenceToString(hunter->equipment, equip);
        printf("%s |", equip);
        printf(" Boredom: %d |", hunter->boredemTimer);
        printf(" Fear: %d |", hunter->fear);
        printf(" Curent Room: %s |\n", hunter->room ? hunter->room->name: "undefined");
    
        currNode = currNode->nextNode;
    }
}

/*
Function - printGhost 
Purpose - Prints the ghost and its data  
out - ghost (GhostType*)
*/
void printGhost(GhostType *ghost) {
    char entity[MAX_STR];
    ghostToString(ghost->ghostType, entity);
    printf("\nGhost: %s |", entity);
    printf(" Boredom: %d |", ghost->boredemTimer);
    printf(" Curent Room: %s |", ghost->room ? ghost->room->name: "undefined");
    for (int i=0;i<3;i++) {
        char evidenceNew[MAX_STR];
        evidenceToString(ghost->evidence[i], evidenceNew);
        printf(" %s |", evidenceNew);
    }
}

/*
Function - printConnections 
Purpose - Prints the given rooms adjacency list  
out - room (RoomType*)
*/
void printConnections(RoomType *room) {
    printf("\nRooms connected to: %s |", room->name);

    RNodeType *nextNode = room->rooms->head;
    printf(" Connections: ");
    while (nextNode != NULL) {
        printf("%s, ", nextNode->currRoomObj->name);
        nextNode = nextNode->nextNode;
    }
    printf("\n");
}