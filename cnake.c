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
    
    while(gameBoard[applePos.x][applePos.y] != 0){
        applePos.x = randomize_position();
        applePos.y = randomize_position();
    }

    gameBoard[applePos.x][applePos.y] = -1;

    if(DEBUG)
        fprintf(stderr, "New apple position: %d,%d\n", applePos.x, applePos.y);
}

void resetGame(){
    memset(&gameBoard, 0, sizeof(gameBoard[0][0]) * BOARD_SIZE_SQRD);
    snakePos.x = BOARD_SIZE / 2 - 1;
    snakePos.y = BOARD_SIZE / 2 - 1;
    snakeLength = STARTING_LENGTH;
    direction = DOWN;

    for(int i = snakeLength; i > 0; i--)
        gameBoard[snakePos.y - i][snakePos.x] = STARTING_LENGTH - i;

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


int main(int argc, char **argv){
    initSettings(); // On Windows, runs some unique code. On Unix, this is a method stub.
    printf("\x1b[2J"); // ANSI code ESC[2J: clear the screen
    srand(time(0));

    if(argc > 1 && !strcmp(argv[1], "-debug"))
        DEBUG = 1;
    
    resetGame();
    displayBoard();

    for(;;){
        char response = '\0';
        printf("Press space to start, or Q to quit\n");
        while(response != ' '){
            response = getch();
            if(response == 'q'){
                goto cleanup;
                break;
            }
        }

        inputServerInit();

        printf("\x1b[2J");

        enum GAME_STATE state = gameTick();

        while(state == OK){
            displayBoard();
            
            sleep_msec(SLEEP_TIME);

            state = gameTick();
        }
        
        inputServerStop();
        printf("Score: %d\n", snakeLength - STARTING_LENGTH);
        resetGame();
    }
cleanup:
    printf("\x1b[2J");
    printf("\x1b[H");

    return 0;
}