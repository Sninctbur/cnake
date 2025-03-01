#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include "cnake.h"
#include "inputserver.c"


int gameBoard[BOARD_SIZE][BOARD_SIZE];
struct vector2 snakePos, applePos;
int snakeLength;
bool DEBUG;
enum SNAKE_DIR direction;

#define RAND_CONSTANT (BOARD_SIZE - APPLE_PADDING * 2)
#define randomize_position() rand() % RAND_CONSTANT + APPLE_PADDING


void setApplePos(){
    applePos.x = randomize_position();
    applePos.y = randomize_position();
    
    while(gameBoard[applePos.y][applePos.x] != 0){
        applePos.x = randomize_position();
        applePos.y = randomize_position();
    }

    if(DEBUG)
        fprintf(stderr, "New apple position: %d,%d\n", applePos.x, applePos.y);
}

void resetGame(){
    memset(&gameBoard, 0, sizeof(gameBoard[0][0]) * BOARD_SIZE_SQRD);
    snakePos.x = BOARD_SIZE / 2 - 1;
    snakePos.y = BOARD_SIZE / 2 - 1;
    snakeLength = STARTING_LENGTH;
    direction = DOWN;

    gameBoard[snakePos.y][snakePos.x] = STARTING_LENGTH;

    setApplePos();
}

enum GAME_STATE gameTick(){
    // Update snake segment duration
    for(int i = 0; i < BOARD_SIZE_SQRD; i++){
        int x = i % BOARD_SIZE,
            y = i / BOARD_SIZE;
        
        if(gameBoard[x][y] > 0)
            gameBoard[x][y]--;
    }

    // Parse user input
    pthread_mutex_lock(&inputMutex);
    switch(input){
        case KEY_UP:
            if(direction != DOWN)
                direction = UP;
            break;
        case KEY_DOWN:
            if(direction != UP)
                direction = DOWN;
            break;
        case KEY_LEFT:
            if(direction != RIGHT)
                direction = LEFT;
            break;
        case KEY_RIGHT:
            if(direction != LEFT)
                direction = RIGHT;
            break;
    }
    pthread_mutex_unlock(&inputMutex);

    // Update snake position
    switch(direction){
        case UP:
            snakePos.y--;
            break;
        case DOWN:
            snakePos.y++;
            break;
        case LEFT:
            snakePos.x--;
            break;
        case RIGHT:
            snakePos.x++;
            break;
        default:
            fprintf(stderr, "wtf\n");
            break;
    }

    // Kill if out of bounds
    if(snakePos.x < 0 || snakePos.x >= BOARD_SIZE
        || snakePos.y < 0 || snakePos.y >= BOARD_SIZE)
        return DEAD;
    // Kill if touching snake segment
    if(gameBoard[snakePos.y][snakePos.x] > 0)
        return DEAD;
    
    // Grow if we got the apple
    if(vectorsEqual(snakePos, applePos)){
        snakeLength++;
        setApplePos();
    }

    gameBoard[snakePos.y][snakePos.x] = snakeLength;

    return OK;
}

void displayBoard(){
    printf("\x1b[H"); // ANSI code ESC[H: return terminal cursor to home position

    for(int x = 0; x < BOARD_SIZE; x++){
        for(int y = 0; y < BOARD_SIZE; y++){
            char c;
            int intAt = gameBoard[x][y];
            
            if(vectorEquals(applePos, y, x))
                c = '@';
            else if(intAt == snakeLength) // Snake head
                c = !DEBUG ? 'S' : intAt + '0';
            else if(intAt > 0)
                c = !DEBUG ? 's' : intAt + '0';
            else
                c = '.';

            printf(" %c ", c);
        }
        putchar(10); // Newline
    }
}

bool isArrowInput(char c){
    for(int i = 0; i < 4; i++)
        if(ARROW_KEYS[i] == c)
            return true;

    return false;
}


int main(int argc, char **argv){
    initSettings(); // On Windows, runs some unique code. On Unix, this is a method stub.
    srand(time(0));

    if(argc > 1 && !strcmp(argv[1], "-debug"))
        DEBUG = 1;
    
    for(;;){
        printf("\x1b[2J"); // ANSI code ESC[2J: clear the screen
        resetGame();
        displayBoard();

        char response;
        printf("Press a direction to start, or Q to quit\n");
        do{
            response = getch();
            if(response == 'q')
                goto cleanup;
        }
        while(!isArrowInput(response));

        printf("\x1b[2J");
        input = response; // No need for a mutex because the input server thread isn't live yet

        inputServerInit();

        enum GAME_STATE state = gameTick();

        while(state == OK){
            displayBoard();
            
            sleep_msec(SLEEP_TIME);

            state = gameTick();
        }

        inputServerStop();

        printf("Score: %d\n", snakeLength - STARTING_LENGTH);
        printf("Press space to retry, or Q to quit\n");
        
        do{
            response = getch();
            if(response == 'q')
                goto cleanup;
        }
        while(response != ' ');
    }
cleanup:
    printf("\x1b[2J");
    printf("\x1b[H");

    return 0;
}