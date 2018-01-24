//
// Created by kumar on 1/23/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>



const int MAX_THREADS = 1024;

long thread_count;
pthread_mutex_t mutex;
pthread_rwlock_t rwlock;
struct list_node_s {
    int data;
    struct list_node_s *next;
};
struct list_node_s *head = NULL;

int n;
int m;
float mMember;
float mInsert;
float mDelete;
int cMember = 0;
int cInsert = 0;
int cDelete = 0;
int cLockMember = 0;
int cLockInsert = 0;
int cLockDelete = 0;
int cSerialMember = 0;
int cSerialInsert = 0;
int cSerialDelete = 0;
int totalMember=0;
int totalDelete=0;
int totalInsert=0;
int noOfTimes;
//double totalSerialTime=0.0;
//double totalMutexTime=0.0;
//double totalLockTime=0.0;

int Member(int value, struct list_node_s *head_p);
int Insert(int value, struct list_node_s **head_pp);
int Delete(int value, struct list_node_s **head_pp);
void Get_args(int argc, char* argv[]);
void *PerformMutex(void *rank);
void *PerformLocks(void *rank);
void *PerformSerial();
void Usage(char *prog_name);

int main(int argc, char *argv[]) {


    long thread;
    pthread_t *thread_handles;
    double startMutex, endMutex, executionMutex;
    double startLocks, endLocks, executionLocks;
    double startSerial, endSerial, executionSerial;
    Get_args(argc, argv);
    int i = 0;
    for (; i < n; i++) {
        int r = rand() % 65536;
        if (!Insert(r, &head)) {
            i--;
        }
    }
    startSerial = clock();
    PerformSerial();
    endSerial = clock();
    executionSerial = (endSerial - startSerial) / CLOCKS_PER_SEC;
//        totalSerialTime+=executionSerial;
    thread_handles = (pthread_t *) malloc(thread_count * sizeof(pthread_t));
    startMutex = clock();
    pthread_mutex_init(&mutex, NULL);
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, PerformMutex, (void *) thread);
    }

    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
    pthread_mutex_destroy(&mutex);
    endMutex = clock();
    executionMutex = (endMutex - startMutex) / CLOCKS_PER_SEC;
//        totalMutexTime+=executionMutex;
    startLocks = clock();
    pthread_rwlock_init(&rwlock, NULL);
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, PerformLocks, (void *) thread);
    }

    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
    pthread_rwlock_destroy(&rwlock);
    endLocks = clock();
    executionLocks = (endLocks - startLocks) / CLOCKS_PER_SEC;
//        totalLockTime+=executionLocks;
    printf("\nSerial = %f seconds\n", executionSerial);
    printf("One Mutex for entireList  = %f seconds\n", executionMutex);
    printf("Read-Write lock = %f seconds\n", executionLocks);
//        cMember = 0;
//        cInsert = 0;
//        cDelete = 0;
//        cLockMember = 0;
//        cLockInsert = 0;
//        cLockDelete = 0;
//        cSerialMember = 0;
//        cSerialInsert = 0;
//        cSerialDelete = 0;
//        totalMember=0;
//        totalDelete=0;
//        totalInsert=0;

//    printf("\nExecution times :\n");
//    printf("Total Serial = %f seconds\n", totalSerialTime);
//    printf("Total One Mutex for entireList  = %f seconds\n", totalMutexTime);
//    printf("Total Read-Write lock = %f seconds\n", totalLockTime);
//    printf("Serial = %f seconds\n", executionSerial);
//    printf("One Mutex for entireList  = %f seconds\n", executionMutex);
//    printf("Read-Write lock = %f seconds\n", executionLocks);
//    printf("sum member%d\n",cMember);
//    printf("sum insert%d\n",cInsert);
//    printf("sum delete%d\n",cDelete);
//    printf("sum member%d\n",cLockMember);
//    printf("sum insert%d\n",cLockInsert);
//    printf("sum delete%d\n",cLockDelete);
} /*main*/


void *PerformMutex(void *rank) {
    while (cMember<=totalMember || cDelete<=totalDelete || cInsert<=totalInsert  ){
        float seperator = (rand() % 3);

        int r = rand() % 65536;
        if (seperator ==0 && cMember<=totalMember) {
            pthread_mutex_lock(&mutex);
            Member(r, head);
            cMember++;
            pthread_mutex_unlock(&mutex);
        } else if (seperator == 1 && cInsert<=totalInsert ) {
            pthread_mutex_lock(&mutex);
            Insert(r, &head);
            cInsert++;
            pthread_mutex_unlock(&mutex);
        } else if (seperator ==2 && cDelete<=totalDelete ){
            pthread_mutex_lock(&mutex);
            Delete(r, &head);
            cDelete++;
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}  /* Thread_mutex */

void *PerformLocks(void *rank) {
    while (cLockMember<=totalMember || cLockDelete<=totalDelete || cLockInsert<=totalInsert  ){
        float seperator = (rand() % 3);

        int r = rand() % 65536;
        if (seperator ==0 && cLockMember<=totalMember) {
            pthread_rwlock_rdlock(&rwlock);
            Member(r,head);
            cLockMember++;
            pthread_rwlock_unlock(&rwlock);
        } else if (seperator == 1 && cLockInsert<=totalInsert ) {
            pthread_rwlock_wrlock(&rwlock);
            Insert(r,&head);
            cLockInsert++;
            pthread_rwlock_unlock(&rwlock);
        } else if (seperator ==2 && cLockDelete<=totalDelete ){
            pthread_rwlock_wrlock(&rwlock);
            Delete(r,&head);
            cLockDelete++;
            pthread_rwlock_unlock(&rwlock);
        }
    }

    return NULL;
}  /* Thread_locks */

void *PerformSerial() {
    while (cSerialMember<=totalMember || cSerialDelete<=totalDelete || cSerialInsert<=totalInsert  ){
        float seperator = (rand() % 3);

        int r = rand() % 65536;
        if (seperator ==0 && cSerialMember<=totalMember) {
            Member(r, head);
            cSerialMember++;
        } else if (seperator == 1 && cSerialInsert<=totalInsert ) {
            Insert(r, &head);
            cSerialInsert++;

        } else if (seperator ==2 && cSerialDelete<=totalDelete ){
            Delete(r, &head);
            cSerialDelete++;
        }
    }

    return NULL;
}  /* Thread_serial */


int Insert(int value, struct list_node_s **head_pp) {
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL) {
            *head_pp = temp_p;
        } else {
            pred_p->next = temp_p;
        }
        return 1;

    } else {
        return 0;
    }
}   /*Insert*/


int Member(int value, struct list_node_s *head_p) {
    struct list_node_s *curr_p = head_p;

    while (curr_p != NULL && curr_p->data < value) {
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        return 0;
    } else {
        return 1;
    }
}/* Member */

int Delete(int value, struct list_node_s **head_pp) {
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data < value) {
        if (pred_p == NULL) {
            *head_pp = curr_p->next;
            free(curr_p);
        } else {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;

    } else {
        return 0;
    }

}   /*Delete*/

void Get_args(int argc, char* argv[]) {
    if (argc != 7) {
        Usage(argv[0]);
    }
    thread_count = strtol(argv[1], NULL, 10);
    if (thread_count <= 0 || thread_count > MAX_THREADS) {
        Usage(argv[0]);
    }

    n = (int) strtol(argv[2], (char **) NULL, 10);
    m = (int) strtol(argv[3], (char **) NULL, 10);

    mMember = (float) atof(argv[4]);
    mInsert = (float) atof(argv[5]);
    mDelete = (float) atof(argv[6]);


    if (n <= 0 || m <= 0 || mMember + mInsert + mDelete != 1.0) Usage(argv[0]);
    totalMember = m*mMember;
    totalInsert = m*mInsert;
    totalDelete = m*mDelete;


}  /* Get_args */



void Usage(char *prog_name) {
    fprintf(stderr, "usage: %s <number of threads> <n> <m> <mMember> <mInsert> <mDelete> <noOfExecutions>\n", prog_name);
    fprintf(stderr, "n : Total Number of Unique Values.\n");
    fprintf(stderr, "m : Total Number of Operations.\n");
    fprintf(stderr, "mMember : Fraction of Member Operations.\n");
    fprintf(stderr, "mInsert : Fraction of Insert Operations.\n");
    fprintf(stderr, "mDelete : Fraction of Delete Operations.\n");
    exit(0);
}  /* Usage */

