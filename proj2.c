#include "proj2.h"

int ArgVal(int argc, char *argv[]){
    if(argc < 5){
        return NOT_ENOUGH_ARGUMENTS;
    }

    if (argc > 5){
        return TOO_MANY_ARGUMENTS;
    }

    if(atoi(argv[1]) < 1){
        return FALSE_VALUE;
    }

    if(atoi(argv[2]) < 1){
        return FALSE_VALUE;
    }

    if(atoi(argv[3]) < 0 || atoi(argv[3]) > 10000){
        return FALSE_VALUE;
    }

    if(atoi(argv[4]) < 0 || atoi(argv[4]) > 100){
        return FALSE_VALUE;
    }

    if(atoi(argv[5]) < 0 || atoi(argv[5]) > 10000){
        return FALSE_VALUE;
    }

}

void ErrorMessage(int errorCode){
    fprintf(stderr, "\033[0;31m"); // Change color to red
    fprintf(stderr, "%s ", "ERROR:"); // Write error in red
    fprintf(stderr, "\033[0m"); // Change color back to default
    switch (errorCode)
    {
    case FALSE_VALUE:
        fprintf(stderr,"Wrong value in argument!");
        break;
    
    case NOT_ENOUGH_ARGUMENTS:
        fprintf(stderr, "Not enough arguments!");
        break;

    case TOO_MANY_ARGUMENTS:
        fprintf(stderr, "There is too many arguments!");
        break;

    case FILE_OPENING_FAILED:
        fprintf(stderr, "Failed to open output file!");
        break;

    case PROCESS_CREATION_FAILED:
        fprintf(stderr, "Failed to create process!");
        break;

    case CREATING_SHARED_MEMO_FAILED:
        fprintf(stderr, "Failed to create shared memory block!");
        break;

    case CREATING_SEMAPHORE_FAILED:
        fprintf(stderr, "Failed to create one or more semaphores!");
        break;
    
    default:
        fprintf(stderr, "Unknown error type!");
        break;
    }
    fprintf(stderr, "\n");
}

void ExitWithError(ErrorCode){
    //cleanup();
    ErrorMessage(ErrorCode);
    exit(1);
}

void initSemaphores(){
    sem_unlink("xkopac07.writing");
    sem_unlink("xkopac07.oxygenQueue");
    sem_unlink("xkopac07.hydrogenQueue");
    sem_unlink("xkopac07.hydrogenStop");
    sem_unlink("xkopac07.moleculeStarted");
    sem_unlink("xkopac07.moleculeFinished");

    writer = sem_open("writer", O_CREAT | O_EXCL, 0666, 1);
    letter_queue = sem_open("letter_queue", O_CREAT | O_EXCL, 0666, 0);
    package_queue = sem_open("package_queue", O_CREAT | O_EXCL, 0666, 0);
    finance_queue = sem_open("finance_queue", O_CREAT | O_EXCL, 0666, 2);
    customer = sem_open("customer", O_CREAT | O_EXCL, 0666, 1);
    worker_available = sem_open("worker_available", O_CREAT | O_EXCL, 0666, 0);
    post_open = sem_open("post_open", O_CREAT | O_EXCL, 0666, 0);

    if (writer == SEM_FAILED || letter_queue == SEM_FAILED || package_queue == SEM_FAILED || finance_queue == SEM_FAILED || customer == SEM_FAILED || worker_available == SEM_FAILED || post_open == SEM_FAILED){
        ExitWithError(CREATING_SEMAPHORE_FAILED);
    }
}

initMemo(){
    
}

void ClearEverything(){
    sem_close(writer);
    sem_close(letter_queue);
    sem_close(finance_queue);
    sem_close(package_queue);
    sem_close(customer);
    sem_close(worker_available);
    sem_close(post_open);

    sem_unlink("writer");
    sem_unlink("letter_queue");
    sem_unlink("package_queue");
    sem_unlink("finance_queue");
    sem_unlink("customer");
    sem_unlink("worker_available");
    sem_unlink("post_open");

}



int main(int argc, char *argv[]){

    //arguments 
    int customer_quantity = atoi(argv[1]);
    int workers_quantity = atoi(argv[2]);
    int customer_wait_time = atoi(argv[3]);
    int worker_break = atoi(argv[4]);
    int post_open_time = atoi(argv[5]);



}