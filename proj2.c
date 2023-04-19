#include "proj2.h"

void ArgVal(int argc, char *argv[]){
    
    if(argc < 5){
        ExitWithError(NOT_ENOUGH_ARGUMENTS);
    }

    if (argc > 5){
        ExitWithError(TOO_MANY_ARGUMENTS);
    }

    if(atoi(argv[1]) < 1){
        ExitWithError(FALSE_VALUE);
    }

    if(atoi(argv[2]) < 1){
        ExitWithError(FALSE_VALUE);
    }

    if(atoi(argv[3]) < 0 || atoi(argv[3]) > 10000){
        ExitWithError(FALSE_VALUE);
    }

    if(atoi(argv[4]) < 0 || atoi(argv[4]) > 100){
        ExitWithError(FALSE_VALUE);
    }

    if(atoi(argv[5]) < 0 || atoi(argv[5]) > 10000){
        ExitWithError(FALSE_VALUE);
    }

    //isAllDigits(argc, argv);
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
    case WRONG_RANDOM_NUMBER:
        fprintf(stderr, "Wrong random number generated!");
        break;
    
    default:
        fprintf(stderr, "Unknown error type!");
        break;
    }
    fprintf(stderr, "\n");
}

void ExitWithError(int ErrorCode){
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
    waiting_customers = sem_open("/xkacka00.waiting_customer", O_CREAT | O_EXCL, 0666, 1);
    worker_available = sem_open("/xkacka00.worker_available", O_CREAT | O_EXCL, 0666, 0);
    post_open = sem_open("/xkacka00.post_open", O_CREAT | O_EXCL, 0666, 0);

    if (writer == SEM_FAILED || letter_line == SEM_FAILED || package_line == SEM_FAILED || finance_line == SEM_FAILED || waiting_customers == SEM_FAILED || worker_available == SEM_FAILED || post_open == SEM_FAILED){
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
    sem_close(waiting_customers);
    sem_close(worker_available);
    sem_close(post_open);

    sem_unlink("/xkacka00.writer");
    sem_unlink("/xkacka00.letter_line");
    sem_unlink("/xkacka00.package_line");
    sem_unlink("/xkacka00.finance_line");
    sem_unlink("/xkacka00.waiting_customer");
    sem_unlink("/xkacka00.worker_available");
    sem_unlink("/xkacka00.post_open");

    munmap(Memo, sizeof(int));
    shm_unlink("/xkacka00.memo");

    close(shared_memo);
    fclose(output);
}

void createCustomer(person_t* person){
    srand(time(NULL) ^ getpid());

    sem_wait(writer);
    fprintf(output, "%d: Z %d: started\n", ++Memo->output_lines, person->id);
    sem_post(writer);

    usleep(1000 * (rand() % customer_wait_time));

    if(office_open){
        int randomNumCustomer = rand() % 10;

        switch (randomNumCustomer % 3) {
        case 0:
            Memo->letter_queue_count++;
            sem_wait(writer);
            fprintf(output, "%d: Z %d: entering office for a service letter\n", ++Memo->output_lines, person->id);
            sem_post(writer);

            sem_wait(letter_line);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: called by office worker\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            Memo->letter_queue_count--;

            usleep((rand() % (10 - 0 + 1)) + 0);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
            sem_post(writer);

            return;
        case 1:
            Memo->package_queue_count++;
            sem_wait(writer);
            fprintf(output, "%d: Z %d: entering office for a service package\n", ++Memo->output_lines, person->id);
            sem_post(writer);

            sem_wait(package_line);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: called by office worker\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            Memo->package_queue_count--;

            usleep((rand() % (10 - 0 + 1)) + 0);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            return;
        case 2:
            Memo->finance_queue_count++;
            sem_wait(writer);
            fprintf(output, "%d: Z %d: entering office for a service finance\n", ++Memo->output_lines, person->id);
            sem_post(writer);

            sem_wait(finance_line);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: called by office worker\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            Memo->finance_queue_count--;

            usleep((rand() % (10 - 0 + 1)) + 0);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            return;
        default:
            ExitWithError(WRONG_RANDOM_NUMBER);
        }
    }else{
        sem_wait(writer);
        fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
        sem_post(writer);

        return;
    }


}

void createWorker(person_t* person){
    srand(time(NULL) ^ getpid());

    int randomNumberWorker = 0;
    sem_wait(writer);
    fprintf(output, "%d: U %d: started\n", ++Memo->output_lines, person->id);
    sem_post(writer);

    do {
        if(letter_line != 0 && letter_line == 0 && letter_line == 0){
            randomNumberWorker = 1;
        }else if(letter_line == 0 && letter_line != 0 && letter_line == 0){
            randomNumberWorker = 2;
        }else if(letter_line == 0 && letter_line == 0 && letter_line != 0){
            randomNumberWorker = 3;
        }else if(letter_line != 0 && letter_line != 0 && letter_line == 0){
            randomNumberWorker = (1 + rand() % (2 - 1 + 1));
        }else if(letter_line != 0 && letter_line == 0 && letter_line != 0){
            randomNumberWorker = (1 + rand() % (3 - 1 + 1));
        }else if(letter_line == 0 && letter_line != 0 && letter_line != 0){
            randomNumberWorker = (2 + rand() % (3 - 2 + 1));
        }else if(letter_line != 0 && letter_line != 0 && letter_line != 0){
            randomNumberWorker = ((rand() % 3) + 1);
        }else {
            sem_wait(writer);
            fprintf(output, "%d: U %d: taking a break\n", ++Memo->output_lines, person->id);
            sem_post(writer);

            usleep(1000 * (rand() % worker_break));

            sem_wait(writer);
            fprintf(output, "%d: U %d: finished break\n", ++Memo->output_lines, person->id);
            sem_post(writer);
        }

        if(randomNumberWorker != 0){
            switch(randomNumberWorker){
                case 1:
                    sem_wait(writer);
                    fprintf(output, "%d: U %d: serving a service letter\n", ++Memo->output_lines, person->id);
                    sem_post(writer);

                    sem_post(letter_line);

                    usleep((rand() % (10 - 0 + 1)) + 0);

                    sem_wait(writer);
                    fprintf(output, "%d: U %d: service finished\n", ++Memo->output_lines, person->id);
                    sem_post(writer);
                    return;
                case 2:
                    sem_wait(writer);
                    fprintf(output, "%d: U %d: serving a service package\n", ++Memo->output_lines, person->id);
                    sem_post(writer);

                    sem_post(package_line);

                    usleep((rand() % (10 - 0 + 1)) + 0);

                    sem_wait(writer);
                    fprintf(output, "%d: U %d: service finished\n", ++Memo->output_lines, person->id);
                    sem_post(writer);
                    return;
                case 3:
                    sem_wait(writer);
                    fprintf(output, "%d: U %d: serving a service finance\n", ++Memo->output_lines, person->id);
                    sem_post(writer);

                    sem_post(finance_line);

                    usleep((rand() % (10 - 0 + 1)) + 0);

                    sem_wait(writer);
                    fprintf(output, "%d: U %d: service finished\n", ++Memo->output_lines, person->id);
                    sem_post(writer);
                    return;
            }
        }
    
    } while ((letter_line != 0 || letter_line != 0 || letter_line != 0) && office_open == true);

    sem_wait(writer);
    fprintf(output, "%d: U %d: going home\n", ++Memo->output_lines, person->id);
    sem_post(writer);
    return;
}

int main(int argc, char *argv[]){
    //validate arguments 
    //ArgVal(argc, argv);

    if(argc == 0){
        printf("skap");
    }
    //arguments 
    customer_quantity = atoi(argv[1]);
    workers_quantity = atoi(argv[2]);
    customer_wait_time = atoi(argv[3]);
    worker_break = atoi(argv[4]);
    post_open_time = atoi(argv[5]);

    office_open = false;

    //person struct init
    person_t person;
    
    initMemo();
    initSemaphores();
    OpenFile();

    
    //making workers processes 
    for ( int workerID = 1; workerID <= workers_quantity; workerID++)
    {
        person.type = WORKER;
        person.id = workerID;

        //create subproces
        pid_t worker = fork();
        if (worker < 0){
            ExitWithError(PROCESS_CREATION_FAILED);
        }
        if (worker == 0){
            createWorker(&person);
            return GOOD;
        }

    }

    for (int customerID = 1; customerID <= customer_quantity; customerID++)
    {
        person.type = CUSTOMER;
        person.id = customerID;

        //create process
        pid_t customer = fork();

        if (customer < 0){
            ExitWithError(PROCESS_CREATION_FAILED);
        }
        if (customer == 0){
            createCustomer(&person);
            return GOOD;
        }
    }
    int low_interval = post_open_time / 2;
    int random_time = (rand() % (post_open_time - low_interval + 1)) + low_interval;
    office_open = true; 
    usleep(random_time);
    office_open = false;
    
    while(wait(NULL) > 0)
        continue;
    
    sem_wait(writer);
    fprintf(output, "%d: closing\n", ++Memo->output_lines);
    sem_post(writer);

    ClearEverything();

    exit(0);
}