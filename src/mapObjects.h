#ifndef MAP_H
#define MAP_H

#define SNAKE_LENGTH   256
//#define SQUARE_SIZE     25
#define FOOD_ITEMS      10

//----------------------------------------------------------------------------------
// Configuration variables
//----------------------------------------------------------------------------------

static const int mapWidth = 3000;
static const int mapHeight = 1600;
static int borderWidth = 40;  
static int offMapSize = 110; //how many blocks to fit outside the map in the screen when near borders

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Snake {
    Vector2 position;
    float size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    float size;
    bool active;
    Color color;
    int points;
    float lifetime;
} Food;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern int score;
extern float snakeSpeedX, snakeSpeedY;

//----------------------------------------------------------------------------------
// Map Functions Declaration
//----------------------------------------------------------------------------------
void InitMap(void);
void CalcFruitPos();
Food* GetFoodObject(int i);
void DrawMap(void);
void UnloadMap(void);

//----------------------------------------------------------------------------------
// Snake Functions Declaration
//----------------------------------------------------------------------------------
void InitSnake(void);
void SetSnakeAsCameraTarget(Camera2D *camera);
void UpdateMovement(Camera2D *camera);
bool CalcWallCollision(void);
bool CalcSelfCollision(void);
void CalcFruitCollision(void);
void DrawSnake(void);
void MoveSnake(void);
bool FruitIsOnSnake(Food fruit);
void UpdateCameraCenterInsideMap(Camera2D *camera, int screenWidth, int screenHeight);

#endif