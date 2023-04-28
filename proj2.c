/**
 * @file proj2.c
 * @author Jakub Kačka (xkacka00@stud.fit.vutbr.cz)
 * @brief Implementation of project 2 for IOS
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "proj2.h"

/**
 * @brief Function for validating arguments
 * 
 * @param argc 
 * @param argv 
 */
void ArgVal(int argc, char *argv[]){
    
    if(argc < 6){
        ExitWithError(NOT_ENOUGH_ARGUMENTS);
    }

    if (argc > 6){
        ExitWithError(TOO_MANY_ARGUMENTS);
    }

    if(atoi(argv[1]) < 0){
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

    isAllDigits(argc, argv);
}

/**
 * @brief Function for checking if all arguments are digits
 * 
 * @param argc 
 * @param argv 
 */
void isAllDigits(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        for (int j = 0; argv[i][j]; j++) {
            if (!isdigit(argv[i][j])) {
                ExitWithError(FALSE_VALUE);
            }
        }
    }
}

/**
 * @brief Function for printing error message
 * 
 * @param errorCode 
 */
void ErrorMessage(int errorCode){

    //changing color of text to red
    fprintf(stderr, "\033[0;31m"); 
    fprintf(stderr, "%s ", "ERROR:");
    fprintf(stderr, "\033[0m"); 

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
    ErrorMessage(ErrorCode);
    exit(1);
}

/**
 * @brief Function for initializing semaphores
 * 
 */
void initSemaphores(){
    sem_unlink("/xkacka00.writer");
    sem_unlink("/xkacka00.letter_line");
    sem_unlink("/xkacka00.package_line");
    sem_unlink("/xkacka00.finance_line");
    sem_unlink("/xkacka00.post_open");
    sem_unlink("/xkacka00.picker");

    writer = sem_open("/xkacka00.writer", O_CREAT | O_EXCL, 0666, 1);
    letter_line = sem_open("/xkacka00.letter_line", O_CREAT | O_EXCL, 0666, 0);
    package_line = sem_open("/xkacka00.package_line", O_CREAT | O_EXCL, 0666, 0);
    finance_line = sem_open("/xkacka00.finance_line", O_CREAT | O_EXCL, 0666, 0);
    post_open = sem_open("/xkacka00.post_open", O_CREAT | O_EXCL, 0666, 0);
    picker = sem_open("/xkacka00.picker", O_CREAT | O_EXCL, 0666, 1);

    if (writer == SEM_FAILED || letter_line == SEM_FAILED || package_line == SEM_FAILED || finance_line == SEM_FAILED || post_open == SEM_FAILED || picker == SEM_FAILED){
        ClearMemo();
        ClearSemaphore();
        ExitWithError(CREATING_SEMAPHORE_FAILED);
    }
}

/**
 * @brief Function for initializing shared memory
 * 
 */
void initMemo(){
    shared_memo = shm_open("/xkacka00.memo", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR );
    if (shared_memo == -1){
        if (errno == EEXIST){
            ClearMemo();
            shared_memo = shm_open("/xkacka00.memo", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR );
            if (shared_memo == -1){
                ExitWithError(CREATING_SHARED_MEMO_FAILED);
            }
        }else{
            ExitWithError(CREATING_SHARED_MEMO_FAILED);
        }
    }
    ftruncate(shared_memo, sizeof(Memo_t));
    Memo = mmap(NULL, sizeof(Memo_t), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memo, 0);
    if (Memo == MAP_FAILED){
        ClearMemo();
        ExitWithError(CREATING_SHARED_MEMO_FAILED);
    }

    // Memo values initialization
    Memo->finance_queue_count = 0;
    Memo->letter_queue_count = 0;
    Memo->output_lines = 0;
    Memo->package_queue_count = 0;
    Memo->office_count = 0;
    Memo->office_open = true;
    return;
}

/**
 * @brief Function for opening output file
 * 
 */
void OpenFile(){
    output = fopen("proj2.out", "w+");
    if(output == NULL || ferror(output)) {
        ClearMemo();
        ClearSemaphore();
        ErrorMessage(FILE_OPENING_FAILED);
    }
    setbuf(output, NULL);
}

/**
 * @brief Function to clear every resource
 * 
 */
void ClearEverything(){
    ClearSemaphore();
    ClearMemo();
    fclose(output);
}

/**
 * @brief Function to clear semaphores
 * 
 */
void ClearSemaphore(){
    sem_close(writer);
    sem_close(letter_line);
    sem_close(finance_line);
    sem_close(package_line);
    sem_close(post_open);
    sem_close(picker);

    sem_unlink("/xkacka00.writer");
    sem_unlink("/xkacka00.letter_line");
    sem_unlink("/xkacka00.package_line");
    sem_unlink("/xkacka00.finance_line");
    sem_unlink("/xkacka00.post_open");
    sem_unlink("/xkacka00.picker");
}

/**
 * @brief Function to clear shared memory
 * 
 */
void ClearMemo(){
    munmap(Memo, sizeof(int));
    shm_unlink("/xkacka00.memo");
    close(shared_memo);
}

/**
 * @brief Function to create customer process
 * 
 * @param person 
 */
void createCustomer(person_t* person){            

    // starting customer process
    sem_wait(writer);
    fprintf(output, "%d: Z %d: started\n", ++Memo->output_lines, person->id);
    sem_post(writer);

    //waiting for subprocesses to start
    sem_post(post_open);

    //random time to wait before entering office
    if (customer_wait_time != 0){
        usleep(1000 * (rand() % customer_wait_time));
    }

    //if office is open process enters office
    if(Memo->office_open){
        int randomNumCustomer = 3;

        switch (rand() % randomNumCustomer) {
        case 0:
            sem_wait(writer);
            //if office closed while process came to office it goes home
            if(!Memo->office_open){
                fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
                sem_post(writer);
                exit(0);
            }
            //else it enters office
            fprintf(output, "%d: Z %d: entering office for a service 1\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            Memo->office_count++;
            Memo->letter_queue_count++;

            // wait for office worker to call customer
            sem_wait(letter_line);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: called by office worker\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            

            usleep((rand() % 10)  * 1000);

            //after service is done customer leaves office
            sem_wait(writer);
            fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
            sem_post(writer);

            exit(0);
        case 1:
            sem_wait(writer);
            if(!Memo->office_open){
                fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
                sem_post(writer);
                exit(0);
            }
            fprintf(output, "%d: Z %d: entering office for a service 2\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            Memo->office_count++;
            Memo->package_queue_count++;

            sem_wait(package_line);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: called by office worker\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            

            usleep((rand() % 10) * 1000);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            exit(0);
        case 2:
            
            sem_wait(writer);
            if(!Memo->office_open){
                fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
                sem_post(writer);
                exit(0);
            }
            fprintf(output, "%d: Z %d: entering office for a service 3\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            Memo->office_count++;
            Memo->finance_queue_count++;

            sem_wait(finance_line);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: called by office worker\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            

            usleep((rand() % 10) * 1000);

            sem_wait(writer);
            fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
            sem_post(writer);

            exit(0);
        default:
            ExitWithError(WRONG_RANDOM_NUMBER);
        }
    }else{
        //if office is closed customer goes home
        sem_wait(writer);
        fprintf(output, "%d: Z %d: going home\n", ++Memo->output_lines, person->id);
        sem_post(writer);

        exit(0);
    }
}

/**
 * @brief Function to create worker process
 * 
 * @param person 
 */
void WorkerWork (sem_t* semaphore, int line, person_t* person){

    //worker going to selected line
    sem_wait(writer);
    fprintf(output, "%d: U %d: serving a service of type %d\n", ++Memo->output_lines, person->id, line);
    sem_post(writer);

    //worker calling customer from the line
    sem_post(semaphore);
    
  
    usleep((rand() % 10) * 1000);

    //worker finishes service and goes to another line if needed
    sem_wait(writer);
    fprintf(output, "%d: U %d: service finished\n", ++Memo->output_lines, person->id);
    sem_post(writer);
}

void createWorker(person_t* person){

    srand(time(NULL) ^ getpid());
    
    int randomNumberWorker;
    
    //worker process starts
    sem_wait(writer);
    fprintf(output, "%d: U %d: started\n", ++Memo->output_lines, person->id);
    sem_post(writer);

    //waiting for subprocesses to start
    sem_post(post_open);
    while (1) {

        //if office is open and lines are empty worker takes a break
        if((Memo->letter_queue_count == 0 && Memo->package_queue_count == 0 && Memo->finance_queue_count == 0) && Memo->office_open == true){
            sem_wait(writer);
            if (Memo->letter_queue_count != 0 || Memo->package_queue_count != 0 || Memo->finance_queue_count != 0){
                sem_post(writer);
                continue;
            }
            if (Memo->office_open != false){
                fprintf(output, "%d: U %d: taking break\n", ++Memo->output_lines, person->id);
                sem_post(writer);

                if(worker_break != 0){
                    usleep(1000 * (rand() % worker_break));
                }

                sem_wait(writer);
                fprintf(output, "%d: U %d: break finished\n", ++Memo->output_lines, person->id);
                sem_post(writer);
                continue;
            }else {
                sem_post(writer);
                continue;
            }
        }

        //if office is closed and lines are empty worker goes home
        if ((Memo->finance_queue_count == 0 && Memo->letter_queue_count == 0 && Memo->package_queue_count == 0) && Memo->office_open == false){
            sem_wait(writer);
            fprintf(output, "%d: U %d: going home\n", ++Memo->output_lines, person->id);
            sem_post(writer);
            exit(0);
        }

        randomNumberWorker = 0;
        
        //if office is open and lines are not empty worker chooses line
        if(Memo->letter_queue_count != 0 && Memo->package_queue_count == 0 && Memo->finance_queue_count == 0){
            randomNumberWorker = 1;
        }else if(Memo->letter_queue_count == 0 && Memo->package_queue_count != 0 && Memo->finance_queue_count == 0){
            randomNumberWorker = 2;
        }else if(Memo->letter_queue_count == 0 && Memo->package_queue_count == 0 && Memo->finance_queue_count != 0){
            randomNumberWorker = 3;
        }else if(Memo->letter_queue_count != 0 && Memo->package_queue_count != 0 && Memo->finance_queue_count == 0){
            randomNumberWorker = (1 + rand() % (2 - 1 + 1));
        }else if(Memo->letter_queue_count != 0 && Memo->package_queue_count == 0 && Memo->finance_queue_count != 0){
            randomNumberWorker = (1 + rand() % (3 - 1 + 1));
        }else if(Memo->letter_queue_count == 0 && Memo->package_queue_count != 0 && Memo->finance_queue_count != 0){
            randomNumberWorker = (2 + rand() % (3 - 2 + 1));
        }else if(Memo->letter_queue_count != 0 && Memo->package_queue_count != 0 && Memo->finance_queue_count != 0){
            randomNumberWorker = ((rand() % 3) + 1);
        }

        //worker goes to picked line and do his job
        if(randomNumberWorker != 0){    

            switch(randomNumberWorker){
                case 1:
                    if (Memo->letter_queue_count == 0){
                        break;
                    }
                    Memo->letter_queue_count--;
                    WorkerWork(letter_line, 1, person);
                    break;
                case 2:
                    if (Memo->package_queue_count == 0){
                        break;
                    }
                    Memo->package_queue_count--;
                    WorkerWork(package_line, 2, person);
                    break;
                case 3:
                    if (Memo->finance_queue_count == 0){
                        break;
                    }
                    Memo->finance_queue_count--;
                    WorkerWork(finance_line, 3, person);
                    break;
            }
            randomNumberWorker = 0;
        }
    };
}

/**
 * @brief Main function
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]){
    //validate arguments 
    ArgVal(argc, argv);

    //parsing arguments into variables
    customer_quantity = atoi(argv[1]);
    workers_quantity = atoi(argv[2]);
    customer_wait_time = atoi(argv[3]);
    worker_break = atoi(argv[4]);
    post_open_time = atoi(argv[5]);

    //person struct init
    person_t person;

    //init resources
    initMemo();
    initSemaphores();

    //open output file
    OpenFile();

    
    //making workers processes 
    for ( int workerID = 1; workerID <= workers_quantity; workerID++)
    {
        person.type = WORKER;
        person.id = workerID;

        //create subproces
        pid_t worker = fork();
        srand(time(NULL) ^ getpid());

        if (worker < 0){
            ClearEverything();
            ExitWithError(PROCESS_CREATION_FAILED);
        }
        if (worker == 0){
            createWorker(&person);
        }

    }

    for (int customerID = 1; customerID <= customer_quantity; customerID++)
    {
        person.type = CUSTOMER;
        person.id = customerID;

        //create process
        pid_t customer = fork();
        srand(time(NULL) ^ getpid());

        if (customer < 0){
            ClearEverything();
            ExitWithError(PROCESS_CREATION_FAILED);
        }
        if (customer == 0){
            createCustomer(&person);
        } 
    }
    int low_interval = post_open_time / 2;
    int random_time = (rand() % (post_open_time - low_interval + 1)) + low_interval;
    
    //wait for all processes to be created
    for (int i = 0; i < (workers_quantity + customer_quantity); i++) {
        sem_wait(post_open); 
    }
    
    //wait for random time in interval <post_open_time/2, post_open_time> to close office
    usleep(1000 * random_time);
    sem_wait(writer);
    Memo->office_open = false;
    fprintf(output, "%d: closing\n", ++Memo->output_lines);
    sem_post(writer);

    //wait for all processes to finish
    while(wait(NULL) > 0){
         continue;
    }
    
    //clear all resources
    ClearEverything();

    exit(0);
}