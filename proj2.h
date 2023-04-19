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


#define WORKER 'W'
#define CUSTOMER 'C'
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
    CREATING_SEMAPHORE_FAILED,
    WRONG_RANDOM_NUMBER
};

// Semaphores 
sem_t *writer;
sem_t *letter_line;
sem_t *package_line;
sem_t *finance_line;
sem_t *waiting_customers;
sem_t *worker_available;
sem_t *post_open;

// person struct
typedef struct person {
    int id;
    char type;
} person_t;

// arguments
int customer_quantity;
int workers_quantity;
int customer_wait_time;
int worker_break;
int post_open_time;

typedef struct Memo {
    int customer_count;
    int worker_count;
    int letter_queue_count;
    int package_queue_count;
    int finance_queue_count;
    int output_lines;
} Memo_t ;

//postoffice open hours 
bool office_open;

FILE *output;

Memo_t *Memo;
int shared_memo;


void ArgVal(int argc, char *argv[]);
void isAllDigits(int argc, char *argv[]);
void ErrorMessage(int errorCode);
void ExitWithError(int ErrorCode);
void initSemaphores();
void initMemo();
void OpenFile();
void ClearEverything();
int main(int argc, char *argv[]);
