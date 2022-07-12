#ifndef MAP_H
#define MAP_H

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SNAKE_LENGTH   512
#define FOOD_ITEMS      100

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum FoodType { REGULAR, BONUS, BOOST, TAILCUT } FoodType;

typedef struct Snake {
    Vector2 position;
    float boostCapacity;
    float size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    Texture2D* foodTexture;
    float scale;
    int foodType;
    bool active;
    int points;
    int tailIncreaseSize;
    float lifetime;
} Food;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern int score;
extern int counterTail;

extern Food fruits[FOOD_ITEMS];
extern Snake snake[SNAKE_LENGTH];

extern const int mapWidth;
extern const int mapHeight;
extern int borderWidth;
extern int offMapSize;


//------------------------------------------------------------------------------------
// Main Functions Declaration
//------------------------------------------------------------------------------------
void InitGame(void);         // Initialize game
void UpdateGame(void);       // Update game (one frame)
void DrawGame(void);         // Draw game (one frame)
void UnloadGame(void);       // Unload game
void UpdateDrawFrame(void);  // Update and Draw (one frame)
void DrawUI(void);

//----------------------------------------------------------------------------------
// Map Functions Declaration
//----------------------------------------------------------------------------------
void InitMap(void);
void CalcFruitPos(void);
void DrawMap(void);
void UnloadMap(void);
void UpdateCameraCenterInsideMap(Camera2D *camera, int screenWidth, int screenHeight);

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

#endif