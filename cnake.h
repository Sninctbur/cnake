#define SLEEP_TIME 100.0
#define BOARD_SIZE 12
#define STARTING_LENGTH 3
#define BOARD_SIZE_SQRD BOARD_SIZE * BOARD_SIZE

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

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