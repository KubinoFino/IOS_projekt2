/**
 * @file proj2.c
 * @author Jakub Kačka (xkacka00@stud.fit.vutbr.cz)
 * @brief Implementation of project 2 for IOS
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

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
sem_t *picker;
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

//shared memory
typedef struct Memo {
    int letter_queue_count;
    int package_queue_count;
    int finance_queue_count;
    int office_count;
    int output_lines;
    bool office_open;
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
void ClearMemo();
void ClearSemaphore();
void createWorker(person_t* person);
void WorkerWork (sem_t* semaphore, int line, person_t* person);
void createCustomer(person_t* person);
int main(int argc, char *argv[]);
