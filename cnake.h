#define SLEEP_TIME 100.0
#define BOARD_SIZE 12
#define STARTING_LENGTH 3
#define BOARD_SIZE_SQRD BOARD_SIZE * BOARD_SIZE
#define APPLE_PADDING 1

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

// Platform specification
#ifndef _WIN32
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>
#define UNIX 1
int kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;
 
    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }
 
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void initSettings(){}

#else
#include <windows.h>
#include <conio.h>
#define UNIX 0
#define sleep(time) Sleep(time)

int initSettings(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return GetLastError();
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return GetLastError();
    }
    return 0;
}
#endif


enum GAME_STATE {
    OK,
    DEAD
};

enum SNAKE_DIR {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct vector2 {
    int x, y;
};
int vectorsEqual(struct vector2 a, struct vector2 b){
    return a.x == b.x && a.y == b.y;
}
int vectorEquals(struct vector2 v, int x, int y){
    return v.x == x & v.y == y;
}