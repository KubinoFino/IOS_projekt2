#include "proj2.h"

void ArgVal(int argc, char *argv[]){
    
    isAllDigits(argc, *argv);
    
    if(argc < 5){
        ErrorMessage(NOT_ENOUGH_ARGUMENTS);
    }

    if (argc > 5){
        ErrorMessage(TOO_MANY_ARGUMENTS);
    }

    if(atoi(argv[1]) < 1){
        ErrorMessage(FALSE_VALUE);
    }

    if(atoi(argv[2]) < 1){
        ErrorMessage(FALSE_VALUE);
    }

    if(atoi(argv[3]) < 0 || atoi(argv[3]) > 10000){
        ErrorMessage(FALSE_VALUE);
    }

    if(atoi(argv[4]) < 0 || atoi(argv[4]) > 100){
        ErrorMessage(FALSE_VALUE);
    }

    if(atoi(argv[5]) < 0 || atoi(argv[5]) > 10000){
        ErrorMessage(FALSE_VALUE);
    }
}

void isAllDigits(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        for (int j = 0; argv[i][j]; j++) {
            if (!isdigit(argv[i][j])) {
                ErrorMessage(FALSE_VALUE);
            }
        }
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
    ClearEverything();
    ErrorMessage(ErrorCode);
    exit(1);
}

void initSemaphores(){
    sem_unlink("/xkacka00.writer");
    sem_unlink("/xkacka00.letter_line");
    sem_unlink("/xkacka00.package_line");
    sem_unlink("/xkacka00.finance_line");
    sem_unlink("/xkacka00.waiting_customer");
    sem_unlink("/xkacka00.worker_available");
    sem_unlink("/xkacka00.post_open");

    writer = sem_open("/xkacka00.writer", O_CREAT | O_EXCL, 0666, 1);
    letter_line = sem_open("/xkacka00.letter_line", O_CREAT | O_EXCL, 0666, 0);
    package_line = sem_open("/xkacka00.package_line", O_CREAT | O_EXCL, 0666, 0);
    finance_line = sem_open("/xkacka00.finance_line", O_CREAT | O_EXCL, 0666, 0);
    waiting_customer = sem_open("/xkacka00.waiting_customer", O_CREAT | O_EXCL, 0666, 1);
    worker_available = sem_open("/xkacka00.worker_available", O_CREAT | O_EXCL, 0666, 0);
    post_open = sem_open("/xkacka00.post_open", O_CREAT | O_EXCL, 0666, 0);

    if (writer == SEM_FAILED || letter_line == SEM_FAILED || package_line == SEM_FAILED || finance_line == SEM_FAILED || waiting_customer == SEM_FAILED || worker_available == SEM_FAILED || post_open == SEM_FAILED){
        ExitWithError(CREATING_SEMAPHORE_FAILED);
    }
}

void initMemo(){
    shared_memo = shm_open("/xkacka00.memo", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR );
    if (shared_memo == -1){
        ExitWithError(CREATING_SHARED_MEMO_FAILED);
    }
    ftruncate(shared_memo, sizeof(Memo_t));
    Memo = mmap(NULL, sizeof(Memo_t), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memo, 0);
    if (Memo == MAP_FAILED){
        ExitWithError(CREATING_SHARED_MEMO_FAILED);
    }

    // Memo init
    Memo->customer_count = customer_quantity;
    Memo->finance_queue_count = 0;
    Memo->letter_queue_count = 0;
    Memo->output_lines = 0;
    Memo->package_queue_count = 0;
    Memo->worker_count = workers_quantity;
}

void OpenFile(){
    output = fopen("proj2.out", "w+");
    if(output == NULL || ferror(output)) {
        ErrorMessage(FILE_OPENING_FAILED);
    }
    setbuf(output, NULL);
}

void ClearEverything(){
    sem_close(writer);
    sem_close(letter_line);
    sem_close(finance_line);
    sem_close(package_line);
    sem_close(waiting_customer);
    sem_close(worker_available);
    sem_close(post_open);

    sem_unlink("/xkacka00.writer");
    sem_unlink("/xkacka00.letter_line");
    sem_unlink("/xkacka00.package_line");
    sem_unlink("/xkacka00.finance_line");
    sem_unlink("/xkacka00.waiting_customer");
    sem_unlink("/xkacka00.worker_available");
    sem_unlink("/xkacka00.post_open");

    munmap(shared_memo, sizeof(int));
    shm_unlink("/xkacka00.memo");

    close(Memo);
    fclose(output);
}

int main(int argc, char *argv[]){

    //validate arguments 
    ArgVal(argc, *argv);

    //arguments 
    customer_quantity = atoi(argv[1]);
    workers_quantity = atoi(argv[2]);
    customer_wait_time = atoi(argv[3]);
    worker_break = atoi(argv[4]);
    post_open_time = atoi(argv[5]);



}