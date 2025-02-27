#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

char input;
bool active;

pthread_t thread;
pthread_mutex_t inputMutex, activeMutex;


void *inputServerThread(){
    pthread_mutex_lock(&activeMutex);
    while(active){
        pthread_mutex_unlock(&activeMutex);

        if(kbhit()){
            char ch = getch();

            pthread_mutex_lock(&inputMutex);
            input = ch;
            pthread_mutex_unlock(&inputMutex);

            pthread_mutex_lock(&activeMutex);
        }
    }

    pthread_mutex_unlock(&activeMutex);
    return NULL;
}

void inputServerInit(){
    if(pthread_mutex_init(&inputMutex, NULL) != 0)
        fprintf(stderr, "inputMutex init failed");
    
    active = true;
    pthread_create(&thread, NULL, &inputServerThread, NULL);
}

void inputServerStop(){
    active = false;
    pthread_join(thread, NULL);
}