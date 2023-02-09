// Leo Liang
// CS5700
// 1/25/2023

#include "queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// create new process with identifier i, name n and priority pri
process_t* newProcess (int i, char* n, int pri) {
  process_t* p = NULL;                     // temp pointer to hold new process
  p = (process_t*)malloc(sizeof(process_t*));  // create new process
  if (p != NULL) {
    p->identifier = i;                        // put data in process
    p->name = n;
    p->priority = pri;
  }
  return p;
};

void freeProcess (process_t* p) {
  if (p != NULL) {
    free(p);
  }
  return;
};



// create new node with value d and NULL left & right pointers
node_t* newNode (void* d) {
  node_t* n_p = NULL;                     // temp pointer to hold new node
  n_p = (node_t*)malloc(sizeof(node_t));  // create new node
  if (n_p != NULL) {
    n_p->data = d;                        // put data in node
    n_p->left_p = NULL;
    n_p->right_p = NULL;
  }
  return n_p;
};

// free the node pointed to by n_p
// fragile assumption: this function does not free up nodes pointed to by left/right pointers
void freeNode (node_t* n_p) {
  if (n_p != NULL) {
    free(n_p);
  }
  return;
};

// create new empty queue (head and tail are set to NULL)
queue_t* newQueue() {
  queue_t* queue;   // temp pointer to hold newly created queue
  queue = (queue_t*) malloc(sizeof(queue_t));
  queue->head_p=NULL;
  queue->tail_p=NULL;
  queue->size=0;
  return queue;
};

// is the queue empty?
bool isEmpty(queue_t* queue) {
  return queue->size == 0;
};

// function to add a new node with data d to tail of the queue
void enqueue(queue_t* queue, void* element) {
  node_t* n_p = NULL; // temp node pointer
  
  if (queue != NULL) {

    if (isEmpty(queue)) {
      // queue is empty so insertion is easy
      n_p = newNode(element);
      queue->head_p= n_p;
      queue->tail_p= n_p;
      queue->size=1;
    } else {
      // queue is not empty
      n_p = newNode(element);
      n_p->left_p = queue->tail_p;
      queue->tail_p->right_p = n_p;
      queue->tail_p= n_p;
      queue->size++;
    }    
  }
}

void* dequeue(queue_t* queue) {
  node_t* n_p = NULL; // temp node poitner
  void* res;
  if (queue != NULL) {
    n_p = queue->head_p;  // get a pointer to the head of the queue

    if (n_p != NULL) {
      res = n_p->data;      // get the value of data in the head of the queue

      if (queue->head_p  == queue->tail_p) {      
          // only one node in the queue, clear queue head and tail 
          
          queue->head_p = NULL;
          queue->tail_p = NULL;
          queue->size=0;
      } 
      else {
        // mulitple nodes in queue, clean up head pointer and new head of queue
        queue->head_p = n_p->right_p;
        queue->head_p->left_p = NULL;
        queue->size--;
      }
    
      freeNode(n_p);  // free up the node that was dequeued
    }
  } 
  return res;
}

// if queue is not empty, then clean it out -- then free the queue struct
void freeQueue(queue_t* q_p) {
  if (q_p != NULL) {
    // make sure the queue is empty
    while (!isEmpty(q_p)) {
      dequeue(q_p);
    }

    // free up the queue itself
    free(q_p);
  }
  return;
};

process_t* dequeueProcess(queue_t* queue) {
  node_t* n_p = NULL; // temp node poitner
  node_t* max = NULL;
  process_t* res;
  if (queue != NULL) {
    n_p = queue->head_p;  // get a pointer to the head of the queue

    if (n_p != NULL) {
      res = n_p->data;      // get the value of data in the head of the queue

      if (queue->head_p  == queue->tail_p) {      
          // only one node in the queue, clear queue head and tail 
          queue->head_p = NULL;
          queue->tail_p = NULL;
          queue->size = 0;
      } 
      else {
        // mulitple nodes in queue, clean up head pointer and new head of queue
        max = n_p;
        while (n_p != NULL) {
          if (((process_t*)n_p->data)->priority > ((process_t*)max->data)->priority) {
            max = n_p;
          }
          n_p = n_p->right_p;
        }
        if (max->left_p != NULL) {
          max->left_p->right_p = max->right_p;
        }
        if (max->right_p!= NULL) {
        max->right_p->left_p = max->left_p;
        }
        res = max->data;
        queue->size--;
        //freeNode(max);
      }
      freeNode(max);  // free up the node that was dequeued
    }
  } 
  return res;
}

int qsize(queue_t* queue) {
  return queue->size;
}