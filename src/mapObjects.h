#ifndef MAP_H
#define MAP_H
//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SNAKE_LENGTH   512
#define FOOD_ITEMS      100

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum FoodType { REGULAR, BONUS, BOOST, TAILCUT } FoodType;
enum paletteName {WATER, SAND, ROCK, DIRT, GRASS1, GRASS2, GRASS3};


typedef struct Snake {
    Vector2 position;
    unsigned short tileXPos;
    unsigned short tileYPos;;
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
extern const float tileSize;

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
unsigned char** AssignColors(Color* colors);

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