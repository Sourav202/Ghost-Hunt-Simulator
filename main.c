#include "defs.h"

int main() {
    /*
    NOTE: to test the program without threading, comment out all
    sem and pthread calls, and uncomment all the print statements
    for logging purposes.
    */

    // Initialize the random number generator
    srand(time(NULL));

    // Create a house instance and add all the rooms/room connections
    HouseType house;
    initHouse(&house);
    populateRooms(&house);

    // Add hunters for the simulation
    char namesOfHunters[NUM_HUNTERS][MAX_STR];
    EvidenceType equipment[NUM_HUNTERS];
    int equipmentInput;

    // Get the name and equipment of the hunters
    for (int i = 0; i < NUM_HUNTERS; i++) {
        printf("Please Enter the Hunter Name %d: ", i + 1);
        scanf("%63s", namesOfHunters[i]);
        while (getchar() != '\n');

        printf("Please Enter the Hunter Equipment Scanner %d: (0: EMF, 1: TEMPERATURE, 2: FINGERPRINTS, 3: SOUND): ", i + 1);
        scanf("%d", &equipmentInput);
        while (getchar() != '\n');

        equipment[i] = (EvidenceType)equipmentInput;
    }

    // Initialize the ghost
    GhostType ghost;
    initGhost(&house, &ghost);

    // Initialize and add hunters to the house's hunter list without threads
    HunterType *hunters[NUM_HUNTERS];
    for (int i = 0; i < NUM_HUNTERS; i++) {
        hunters[i] = initHunter(namesOfHunters[i], &equipment[i], house.rooms.head->currRoomObj, &house.sharedEvidence, &house);
        addHunter(hunters[i], &house);
    }

    /*
    pthread_create(&ghost.Gthread, NULL, threadGhost, (void *)&ghost);
    for (int i = 0; i < NUM_HUNTERS; i++) {
        pthread_create(&hunters[i]->Hthread, NULL, threadHunter, (void *)hunters[i]);
    }

    pthread_join(ghost.Gthread, NULL);
    for (int i = 0; i < NUM_HUNTERS; i++) {
        pthread_join(hunters[i]->Hthread, NULL);
    }
    */

    // Print room connections, list, hunters, and ghost for testing purposes
    RNodeType *currNode = house.rooms.head;
    while (currNode != NULL) {
        RoomType *currRoom = currNode->currRoomObj;
        printConnections(currRoom);
        currNode = currNode->nextNode;
    }

    printRoomList(&house.rooms);
    printHunterList(&house.hunters);
    printGhost(&ghost);

    // Clean up the house structure
    cleanupHouse(&house);

    return 0;
}