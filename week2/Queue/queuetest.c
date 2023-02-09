// Leo Liang
// 1/25/2023
// CS5007
#include "queue.h"


int main() {
    // generate new process
    process_t* p1 = newProcess (1, "first", 3);
    process_t* p2 = newProcess (2, "second", 5);
    process_t* p3 = newProcess (3, "third", 1);
    //completely the same
    process_t* p4 = newProcess (1, "first", 3);
    process_t* p5 = newProcess (2, "second", 5);
    process_t* p6 = newProcess (3, "third", 1);
    // completely different
    process_t* p7 = newProcess (5077, "Leo", 2);
    process_t* p8 = newProcess (2044, "Helen", 3);
    process_t* p9 = newProcess (9431, "Enzo", 51);


    // generate new queue
    printf("queue 1\n");
    queue_t* q1_p = newQueue();
    printf("The size of queue without any element is %d\n",q1_p->size);

    // put process into queue
    enqueue(q1_p, p1);
    printf("The size of queue with one element is %d\n",q1_p->size);
    enqueue(q1_p, p2);
    printf("The size of queue with two element is %d\n",q1_p->size);
    enqueue(q1_p, p3);
    printf("The size of queue with three element is %d\n",q1_p->size);

    // test qsize, dequeueProcess and dequeue
    char* temp = ((process_t*)dequeueProcess(q1_p))->name;
    if (temp == "second") {
        printf("With priority the dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeueProcess fail\n");
    }
    
    printf("The size of queue now with one dequeueProcess is %d\n",q1_p->size);
    temp = ((process_t*)dequeue(q1_p))->name;
    if (temp == "first") {
        printf("The next dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeue fail\n");
    }
    
    printf("The size of queue now with one dequeue is %d\n",q1_p->size);
    freeQueue(q1_p);

    // generate new queue
    printf("\nqueue 2\n");
    queue_t* q2_p = newQueue();

    // put process into queue
    enqueue(q2_p, p1);
    printf("The size of queue with one element is %d\n",q2_p->size);
    enqueue(q2_p, p2);
    printf("The size of queue with two element is %d\n",q2_p->size);
    enqueue(q2_p, p3);
    printf("The size of queue with three element is %d\n",q2_p->size);
    enqueue(q2_p, p4);
    printf("The size of queue with four element is %d\n",q2_p->size);
    enqueue(q2_p, p5);
    printf("The size of queue with five element is %d\n",q2_p->size);
    enqueue(q2_p, p6);
    printf("The size of queue with six element is %d\n",q2_p->size);

    /// test qsize, dequeueProcess and dequeue
    temp = ((process_t*)dequeueProcess(q2_p))->name;
    if (temp == "second") {
        printf("With priority the dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeueProcess fail\n");
    }
    
    temp = ((process_t*)dequeueProcess(q2_p))->name;
    if (temp == "second") {
        printf("With priority the dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeueProcess fail\n");
    }

    printf("The size of queue now with two dequeueProcess is %d\n",q1_p->size);

    temp = ((process_t*)dequeue(q2_p))->name;
    if (temp == "first") {
        printf("The next dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeue fail\n");
    }
    
    temp = ((process_t*)dequeue(q2_p))->name;
    if (temp == "third") {
        printf("The next dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeue fail\n");
    }

    printf("The size of queue now with two dequeue is %d\n",q2_p->size);
    freeQueue(q2_p);

    // generate new queue
    printf("\nqueue 3\n");
    queue_t* q3_p = newQueue();

    // put process into queue
    enqueue(q3_p, p7);
    printf("The size of queue with one element is %d\n",q3_p->size);
    enqueue(q3_p, p8);
    printf("The size of queue with two element is %d\n",q3_p->size);
    enqueue(q3_p, p9);
    printf("The size of queue with three element is %d\n",q3_p->size);
    enqueue(q3_p, p1);
    printf("The size of queue with four element is %d\n",q3_p->size);
    enqueue(q3_p, p2);
    printf("The size of queue with five element is %d\n",q3_p->size);
    enqueue(q3_p, p3);
    printf("The size of queue with six element is %d\n",q3_p->size);

    /// test qsize, dequeueProcess and dequeue
    temp = ((process_t*)dequeueProcess(q3_p))->name;
    if (temp == "Enzo") {
        printf("With priority the dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeueProcess fail\n");
    }
    
    temp = ((process_t*)dequeueProcess(q3_p))->name;
    if (temp == "second") {
        printf("With priority the dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeueProcess fail\n");
    }

    printf("The size of queue now with two dequeueProcess is %d\n",q3_p->size);

    temp = ((process_t*)dequeue(q3_p))->name;
    if (temp == "Leo") {
        printf("The next dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeue fail\n");
    }
    
    temp = ((process_t*)dequeue(q3_p))->name;
    if (temp == "Helen") {
        printf("The next dequeue is %s, success\n",temp);
    }
    else {
        printf("dequeue fail\n");
    }

    printf("The size of queue now with two dequeue is %d\n",q3_p->size);
    freeQueue(q3_p);
    return 0;
}