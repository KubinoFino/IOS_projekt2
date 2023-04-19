#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>


// Errors
enum Errors {
    ERROR = -1,
    GOOD,
    FAILED,
    FALSE_VALUE,
    NOT_ENOUGH_ARGUMENTS,
    TOO_MANY_ARGUMENTS,
    FILE_OPENING_FAILED,
    PROCESS_CREATION_FAILED,
    CREATING_SHARED_MEMO_FAILED,
    CREATING_SEMAPHORE_FAILED
};

// Semaphores 
sem_t *writer;
sem_t *letter_queue;
sem_t *package_queue;
sem_t *finance_queue;
sem_t *customer;
sem_t *worker_available;
sem_t *post_open;

typedef struct Memo {
    int customer_count;
    int worker_count;
    int letter_queue_count;
    int package_queue_count;
    int finance_queue_count;
    int output;
};
