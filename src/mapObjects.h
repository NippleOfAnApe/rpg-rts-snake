#ifndef MAP_H
#define MAP_H

#define SNAKE_LENGTH   256
#define SQUARE_SIZE     25
#define FOOD_ITEMS      4

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Snake {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    Vector2 size;
    bool active;
    Color color;
    int points;
    float lifetime;
} Food;
//----------------------------------------------------------------------------------
// Map Functions Declaration
//----------------------------------------------------------------------------------
void InitMap(void);
void CalcFruitPos(Snake snake[], int* counterTail);
bool CalcWallCollision(Snake snake[]);
void CalcFruitCollision(Snake snake[], int* counterTail, Vector2 snakePosition[], int* score);
void DrawMap(void);
void UpdateCameraCenterInsideMap(Camera2D *camera, Snake *player, int screenWidth, int screenHeight);
void UnloadMap(void);
#endif