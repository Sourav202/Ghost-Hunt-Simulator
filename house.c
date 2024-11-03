#include "defs.h"
/*
Function - initHouse
Purpose - calls to function to free a room/huter/evidence list
in - h (HouseType *) 
*/
void initHouse(HouseType *h) {

    initHunterList(&h->hunters);
    initEvidenceList(&h->sharedEvidence);
    initRoomList(&h->rooms);
}

/*
Function - cleanupHouse
Purpose - initializes the house and all its assoicated lists
in/out - h (HouseType *)
*/
void cleanupHouse(HouseType *h) {
    cleanupEvidenceData(&h->sharedEvidence);
    cleanupEvidenceList(&h->sharedEvidence);

    cleanupHunterList(&h->hunters);

    RNodeType *currNode = h->rooms.head;
    while (currNode != NULL) {
        RNodeType *tempNode = currNode;
        currNode = currNode->nextNode;
        cleanupRoomData(tempNode->currRoomObj);
        free(tempNode);
    }
    h->rooms.head = NULL;
    h->rooms.tail = NULL;
}

//base code function
void populateRooms(HouseType* house) {
    // First, create each room

    // createRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // create functions are pretty typical, but it means errors are harder to return aside from NULL
    struct Room* van                = createRoom("Van");
    struct Room* hallway            = createRoom("Hallway");
    struct Room* master_bedroom     = createRoom("Master Bedroom");
    struct Room* boys_bedroom       = createRoom("Boy's Bedroom");
    struct Room* bathroom           = createRoom("Bathroom");
    struct Room* basement           = createRoom("Basement");
    struct Room* basement_hallway   = createRoom("Basement Hallway");
    struct Room* right_storage_room = createRoom("Right Storage Room");
    struct Room* left_storage_room  = createRoom("Left Storage Room");
    struct Room* kitchen            = createRoom("Kitchen");
    struct Room* living_room        = createRoom("Living Room");
    struct Room* garage             = createRoom("Garage");
    struct Room* utility_room       = createRoom("Utility Room");

    // This adds each room to each other's room lists
    // All rooms are two-way connections
    
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    // Add each room to the house's room list
    addRoom(&house->rooms, van);
    addRoom(&house->rooms, hallway);
    addRoom(&house->rooms, master_bedroom);
    addRoom(&house->rooms, boys_bedroom);
    addRoom(&house->rooms, bathroom);
    addRoom(&house->rooms, basement);
    addRoom(&house->rooms, basement_hallway);
    addRoom(&house->rooms, right_storage_room);
    addRoom(&house->rooms, left_storage_room);
    addRoom(&house->rooms, kitchen);
    addRoom(&house->rooms, living_room);
    addRoom(&house->rooms, garage);
    addRoom(&house->rooms, utility_room);
}