#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

char input;
bool active;

pthread_t thread;
pthread_mutex_t inputMutex, activeMutex;

#ifdef _WIN32
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
#else
void *inputServerThread(){
    struct termios old, new;
    tcgetattr(0, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &new);

    pthread_mutex_lock(&activeMutex);
    while(active){
        pthread_mutex_unlock(&activeMutex);

        // TODO: Linux unique input system
        pthread_mutex_lock(&inputMutex);
        input = read(0, &input, 1);
        pthread_mutex_unlock(&inputMutex);
    }

    pthread_mutex_unlock(&activeMutex);
    return NULL;
}

static struct termios termOld, termNew;
#endif

void inputServerInit(){
    if(pthread_mutex_init(&inputMutex, NULL) != 0)
        fprintf(stderr, "inputMutex init failed\n");
    
    active = true;
    pthread_create(&thread, NULL, &inputServerThread, NULL);
}

void inputServerStop(){
    pthread_mutex_lock(&activeMutex);
    active = false;
    pthread_mutex_unlock(&activeMutex);

    pthread_join(thread, NULL);

    pthread_mutex_destroy(&inputMutex);
    pthread_mutex_destroy(&activeMutex);
}