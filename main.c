#include "defs.h"

int main()
{   /*
    NOTE: to test the program without threading comment out all
    sem and pthread calls, and uncomment all the print statement 
    for logging purposes
    */

    // Initialize the random number generator
    srand(time(NULL));

    //create a house instance and add all the rooms/room connections
    HouseType house;
    initHouse(&house);
    populateRooms(&house);

    //add the hunters for the simulation
    char namesOfHunters[NUM_HUNTERS][MAX_STR];
    EvidenceType equipment[NUM_HUNTERS];
    int equipmentInput;
    //gets the name and equipment of the hunters
    for (int i = 0; i < NUM_HUNTERS; i++) {
        printf("Please Enter the Hunter Names: %d: ", i + 1);
        scanf("%63s", namesOfHunters[i]);
        while (getchar() != '\n');

        printf("Please Enter the Hunter Equipment Scanner: %d: (0; EMF, 1: TEMPERATURE, 2: FINGERPRINTS, 3: SOUND)", i + 1);
        scanf("%d", &equipmentInput);
        while (getchar() != '\n');

        equipment[i] = (EvidenceType)equipmentInput;
    }
    //add the ghost for the simulation
    GhostType ghost;
    initGhost(&house, &ghost);

    //initiates and places all 4 hunters inside the starting room
    for (int i = 0; i < NUM_HUNTERS; i++) {
        HunterType *hunter = initHunter(namesOfHunters[i], equipment + i, house.rooms.head->currRoomObj, &(house.sharedEvidence), &house);
        addHunter(hunter, &house);
    }
    //threading and semaphones
    /*
    sem_init(&ghost.room->Mutex, 0, 1);
    pthread_create(&ghost.Gthread, NULL, threadGhost, (void *)&ghost);
    pthread_join(ghost.Gthread, NULL);
    for (int i =0; i < NUM_HUNTERS; i++) {
        HNodeType *currNode = house.hunters.head;
        pthread_create(&currNode->Hunter->Hthread, NULL, threadHunter, (void *)currNode->Hunter);
        pthread_join(currNode->Hunter->Hthread, NULL);
        currNode = currNode->nextNode;
    }
    */
    //prints room connections, list, hunters and ghost
    RNodeType *currNode = house.rooms.head;
    while (currNode != NULL) {
        RoomType *currRoom = currNode->currRoomObj;
        printConnections(currRoom);
        currNode = currNode->nextNode;
    }
    printRoomList(&house.rooms);
    printHunterList(&house.hunters);
    printGhost(&ghost);

    cleanupHouse(&house);
    return 0;
}

