#define SLEEP_TIME 100
#define BOARD_SIZE 12
#define STARTING_LENGTH 3
#define BOARD_SIZE_SQRD BOARD_SIZE * BOARD_SIZE
#define APPLE_PADDING 1

// Platform specification
#ifndef _WIN32
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#define UNIX 1

#define KEY_UP 65
#define KEY_DOWN 66
#define KEY_LEFT 68
#define KEY_RIGHT 67

// This game was originally made for Windows, like a smut
// So we emulate the functions it was using
int rand(){
    return random();
}

int getch(){
    struct termios oldMode, newMode;
    int ch;

    tcgetattr(STDIN_FILENO, &oldMode);
    newMode = oldMode;
    newMode.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newMode);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldMode);

    return ch;
}

int kbhit(){
    struct termios oldMode, newMode;
    int bytesWaiting;

    tcgetattr(STDIN_FILENO, &oldMode);
    newMode = oldMode;
    newMode.c_lflag &= ~(ICANON);

    tcsetattr(STDIN_FILENO, TCSANOW, &newMode);
    ioctl(0, FIONREAD, &bytesWaiting);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldMode);
    return bytesWaiting > 0;
}

int initSettings(){}

#define sleep_msec(time) usleep(time * 1000)

#else
#include <windows.h>
#include <conio.h>
#define UNIX 0
#define sleep_msec(time) Sleep(time)

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

int initSettings(){ // Microsoft code which activates ANSI character support in CMD and PS
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

const int ARROW_KEYS[4] = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};

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