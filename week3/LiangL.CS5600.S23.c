#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int map[120];

void *worker(void *data){
    char *name = (char*)data;
    for (int i = 0; i < 120; i++){
        usleep(50000);
        printf("Hi from thread name = %s%d\n", name,i);
        if (name == "X") {
            map[i] = i;
        }
        else if (name == "Y") {
            printf("map[%d] is %d\n", i, map[i]);
        }
    }
    printf("Thread %s done!\n", name);
    return NULL;
}

int main(void){
    pthread_t th1, th2;
    pthread_create(&th1, NULL, worker, "X");
    pthread_create(&th2, NULL, worker, "Y");
    sleep(1);
    printf("====> Cancelling Thread Y with thread ID: %ld!!\n",th1);
    pthread_cancel(th2);
    usleep(100000);
    printf("====> Cancelling Thread X with thread ID: %ld!!\n",th2);
    pthread_cancel(th1);
    printf("exiting from main program\n");
    return 0;
}

/* problem 4 solution

The behavior from (3) is not consistent and not expected to get consistency,
because there is no particular order between threads to wake up and execute.
They can be seen as running concurrently, and Y may read an empty array when
reads are faster than writes. At the physical level, we cannot predict what a
certain program CPU will run at a time. The CPU converts processes into small 
chunks of work that must be executed according to the schedule. So, unless these
pieces of code are synchronized programmatically, it is impossible to predict what
will happen next. One of the solutions is to add thread lock. Lock before the thread
completes an operation, and the next step cannot be completed until it is unlocked. 
In this way, the order and consistency of operations can be guaranteed.

code for problem 4
int map[120];
pthread_mutex_t lock;

void *worker(void *data){
    char *name = (char*)data;
    for (int i = 0; i < 120; i++){
        usleep(50000);
        printf("Hi from thread name = %s%d\n", name,i);
        if (name == "X") {
            pthread_mutex_lock(&lock);
            map[i] = i;
            pthread_mutex_unlock(&lock);
        }
        else if (name == "Y") {
            pthread_mutex_lock(&lock);
            printf("map[%d] is %d\n", i, map[i]);
            pthread_mutex_unlock(&lock);
        }
    }
    printf("Thread %s done!\n", name);
    return NULL;
}

int main(void){
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    pthread_t th1, th2;
    pthread_create(&th1, NULL, worker, "X");
    pthread_create(&th2, NULL, worker, "Y");
    sleep(1);
    printf("====> Cancelling Thread Y with thread ID: %ld!!\n",th1);
    pthread_cancel(th2);
    usleep(100000);
    printf("====> Cancelling Thread X with thread ID: %ld!!\n",th2);
    pthread_cancel(th1);
    printf("exiting from main program\n");
    pthread_mutex_destroy(&lock);
    return 0;
}
*/

