#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define LOGGING         C_TRUE
#define HUNTER_SIZE     4
#define NUM_OF_ROOMS    12
#define MAX_ARR  128

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);

//--all struct definitions--
typedef   struct House      HouseType;

typedef   struct NodeHunter HNodeType;

typedef   struct Ghost      GhostType;

typedef   struct Room       RoomType;
typedef   struct RoomList   RoomListType;
typedef   struct NodeRoom   RNodeType;

typedef   struct Hunter     HunterType;
typedef   struct HunterList HunterListType;
typedef   struct NodeHunter HNodeType;

typedef   struct EvidenceList EvidenceListType;
typedef   struct NodeEvidence ENodeType;

/*** Complete the GhostType here ***/
struct Ghost {
  EvidenceType  evidence[3];
  RoomType      *room;
  GhostClass    ghostType;
  int           boredemTimer;
  pthread_t     Gthread;
  HouseType     *house;
};

/*** Define the HunterType here ***/
struct Hunter {
  char              name[MAX_STR]; 
  RoomType          *room;
  EvidenceType      equipment;
  int               boredemTimer;
  int               fear;
  EvidenceListType  *evidenceList;
  pthread_t         Hthread;
  HouseType         *house;
};
struct HunterList {
  HNodeType *head;
  HNodeType *tail;
};
struct NodeHunter {
  HunterType *Hunter;
  HNodeType  *nextNode;
};

/*** Define the RoomType here ***/
struct Room {
  char              name[MAX_STR];
  EvidenceListType  *evidences;
  HunterListType    *hunters;
  RoomListType      *rooms;
  GhostType         *ghost;
  sem_t             Mutex;
};
struct RoomList {
  RNodeType  *head;
  RNodeType  *tail;
};
struct NodeRoom {
  RNodeType  *prevNodeRoom;
  RoomType   *currRoomObj;
  RNodeType  *nextNode;
};

/*** Complete the EvidnceType here ***/
struct EvidenceList {
  ENodeType  *head;
  ENodeType  *tail;
  sem_t Mutex;
};
struct NodeEvidence {
  EvidenceType  *evidenceObj;
  ENodeType     *nextNode;
};

/*** Complete the HouseType here ***/
struct House {
  RoomListType      rooms;
  EvidenceListType  sharedEvidence;
  HunterListType    hunters;
};

//--all function definitions--

//ghost functions
void initGhost(HouseType *h, GhostType *ghost);
void moveGhost(GhostType *ghost);
int ghostPresent(RoomType *room);

void cleanupGhostData(GhostType *ghost);

//house functions
void initHouse(HouseType *h);
void cleanupHouse(HouseType *h);

//room functions
void initRoomList(RoomListType *list);
void populateRooms(HouseType *house);
RoomType* createRoom(char *room);
void connectRooms(RoomType *r1, RoomType *r2);
void addRoom(RoomListType *list, RoomType *r);

void cleanupRoomData(RoomType *room);
void cleanupRoomList(RoomListType *list);

//hunter functions
void initHunterList(HunterListType *list);
HunterType* initHunter(char *name, EvidenceType *equipment, RoomType *start, EvidenceListType *sharedEvidenceList, HouseType *house);
void addHunter(HunterType *hunter, HouseType *h);
void setHunterRoom(HunterType *hunter, RoomType *room);
int  hunterPresent(RoomType *room);
void hunterCollect(HunterType *hunter, RoomType *room);
void addEvidenceToSharedHouse(EvidenceListType *houseEvidence, EvidenceType hunterEvidence);
void moveHunter(HunterType *hunter);
void hunterCollect(HunterType *hunter, RoomType *room);
int  reviewHunterEvidence(HunterType *hunter);
int  evidenceExists(EvidenceListType *list, EvidenceType evidence);

void cleanupHunterData(HunterType *hunter);
void cleanupHunterList(HunterListType *list);

//evidence functions
void initEvidenceList(EvidenceListType *list);
void addGhostEvidence(EvidenceType ghostEvidence[], EvidenceListType *roomEvidence, GhostType *ghost);
void addRoomEvidence(EvidenceListType *roomEvidence, EvidenceType ghostEvidence);

void cleanupEvidenceData(EvidenceListType *list);
void cleanupEvidenceList(EvidenceListType *list);

//print functions (testing/debugging)
void printRoomList(RoomListType *list);
void printHunterList(HunterListType *list);
void printGhost(GhostType *ghost);
void printConnections(RoomType *room);

//thread functions
void *threadHunter(void *hArgs);
void *threadGhost(void *gArgs);