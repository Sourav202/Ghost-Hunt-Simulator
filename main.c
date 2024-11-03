#include "defs.h"

int main() {
    srand(time(NULL)); // Initialize the random number generator

    HouseType house;
    initHouse(&house); // Initialize house
    populateRooms(&house); // Populate the house with rooms

    char namesOfHunters[NUM_HUNTERS][MAX_STR];
    EvidenceType equipment[NUM_HUNTERS];
    int equipmentInput;

    for (int i = 0; i < NUM_HUNTERS; i++) {
        printf("Please Enter the Hunter Name %d: ", i + 1);
        scanf("%63s", namesOfHunters[i]);
        while (getchar() != '\n');

        printf("Please Enter the Hunter Equipment Scanner %d: (0: EMF, 1: TEMPERATURE, 2: FINGERPRINTS, 3: SOUND): ", i + 1);
        scanf("%d", &equipmentInput);
        while (getchar() != '\n');

        // Ensure equipmentInput is valid
        if (equipmentInput < 0 || equipmentInput >= EV_COUNT) {
            printf("Invalid equipment choice, defaulting to EMF.\n");
            equipment[i] = EMF;
        } else {
            equipment[i] = (EvidenceType)equipmentInput;
        }
    }

    GhostType ghost;
    initGhost(&house, &ghost); // Initialize ghost

    HunterType *hunters[NUM_HUNTERS];
    for (int i = 0; i < NUM_HUNTERS; i++) {
        hunters[i] = initHunter(namesOfHunters[i], &equipment[i], house.rooms.head->currRoomObj, &house.sharedEvidence, &house);
        addHunter(hunters[i], &house);
    }
    /*
    // Create threads
    pthread_create(&ghost.Gthread, NULL, threadGhost, (void *)&ghost);
    for (int i = 0; i < NUM_HUNTERS; i++) {
        pthread_create(&hunters[i]->Hthread, NULL, threadHunter, (void *)hunters[i]);
    }

    // Wait for threads to finish
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
    

    cleanupHouse(&house); // Clean up the house structure
    return 0;
}