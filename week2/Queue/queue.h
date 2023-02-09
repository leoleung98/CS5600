// Leo Liang
// CS5700
// 1/25/2023
#include <stdio.h>   // stardard input/output library
#include <stdbool.h> // standard boolean library: bool, true, false
#include <stdlib.h>  // library that contains malloc, rand, and srand

// struct for process
typedef struct p {
  int identifier;
  char *name;
  int priority;
} process_t;

process_t* newProcess (int i, char* n, int pri);
void freeProcess (process_t* p);

// struct for node
typedef struct nd {
  void* data;
  struct nd* left_p;
  struct nd* right_p;
} node_t;

node_t* newNode (void* d);
void freeNode (node_t* n_p);

// struct for node
typedef struct q {
  node_t* head_p;
  node_t* tail_p;
  int size;
} queue_t;

queue_t* newQueue();
bool isEmpty(queue_t* queue);
void enqueue(queue_t* queue, void* element);
void* dequeue(queue_t* queue);
void freeQueue(queue_t* q_p);
process_t* dequeueProcess(queue_t* queue);
int qsize(queue_t* queue);