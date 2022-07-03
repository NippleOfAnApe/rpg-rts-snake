#include "raylib.h"
#include "mapObjects.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static Texture2D bgTexture = { 0 };
static Texture2D fgTexture = { 0 };
static Food fruits[FOOD_ITEMS] = { 0 };
static const int mapWidth = 3000;
static const int mapHeight = 1600;

// const int mapHeight = 60;    //squares
// const int mapWidth = 100;
static float bonusFoodLifetime = 10.0f;
static float regularFoodLifetime = 40.0f;
static int bonusFruitPoints = 10;
static int regularFruitPoints = 2;
static int offMapSize = 110; //how many blocks to fit outside the map in the screen when near borders
static int borderWidth = 40;
static int theExtra = 0;    // extra space needed for drawing bg and fg
static float bonusFruitSize = 20.0f;
static float regularFruitSize = 12.0f;

//----------------------------------------------------------------------------------
// Map related Functions Definition
//----------------------------------------------------------------------------------
void InitMap(void)
{
    for (int i = 0; i < FOOD_ITEMS; i++) fruits[i].active = false;

    bgTexture = LoadTexture("../resources/dirtSIZE.png");
    fgTexture = LoadTexture("../resources/grass516.png");
    theExtra = borderWidth * 2 + offMapSize * 2;
}

void CalcFruitPos(Snake snake[], int* counterTail)
{
    for (int i = 0; i < FOOD_ITEMS; i++)
    {
        if (fruits[i].lifetime <= 0) fruits[i].active = false;
        if (!fruits[i].active)
        {
            fruits[i].active = true;
            fruits[i].randomValue = GetRandomValue(1, 20);
        
            //Bonus fruit
            if (fruits[i].randomValue % 10 == 0) 
            {
                fruits[i].size = bonusFruitSize;
                fruits[i].position = (Vector2){ GetRandomValue(fruits[i].size, mapWidth - fruits[i].size), GetRandomValue(fruits[i].size, (mapHeight - fruits[i].size) - 2)};
                fruits[i].points = bonusFruitPoints;
                fruits[i].color = MAROON;
                fruits[i].lifetime = bonusFoodLifetime;
            }
            //Main fruit
            else
            {
                fruits[i].size = regularFruitSize;
                fruits[i].position = (Vector2){ GetRandomValue(fruits[i].size, mapWidth - fruits[i].size), GetRandomValue(fruits[i].size, (mapHeight - fruits[i].size) - 2)};
                fruits[i].points = regularFruitPoints;
                fruits[i].color = YELLOW;
                fruits[i].lifetime = regularFoodLifetime;
            }
            
            for (int i = 0; i < *counterTail; i++)   //To prevent a fruit from spawning on top of a snake
            {
                while ((fruits[i].position.x == snake[i].position.x) && (fruits[i].position.y == snake[i].position.y))
                {
                    fruits[i].position = (Vector2){ GetRandomValue(fruits[i].size, mapWidth - fruits[i].size), GetRandomValue(fruits[i].size, (mapHeight - fruits[i].size) - 2)};
                    i = 0;
                }
            }
        }
        fruits[i].lifetime -= GetFrameTime();
    }
}

bool CalcWallCollision(Snake snake[])
{
    return (((snake[0].position.x - snake[0].size) > (mapWidth - borderWidth)) ||
            ((snake[0].position.y - snake[0].size) > (mapHeight - borderWidth)) ||
            (snake[0].position.x - snake[0].size < 0) || (snake[0].position.y - snake[0].size < 0));
}

void CalcFruitCollision(Snake snake[], int* counterTail, Vector2 snakePosition[], int* score)
{
    for (int i = 0; i < FOOD_ITEMS; i++)
    {
        if (CheckCollisionCircles(snake[0].position, snake[0].size, fruits[i].position, fruits[i].size))
        {
            snake[*counterTail].position = snakePosition[*counterTail - 1];
            *counterTail += 1;
            *score += fruits[i].points;
            fruits[i].active = false;
        }
    }
}

void DrawMap(void)
{
    // BG and FG
    DrawTextureTiled(bgTexture, (Rectangle){0.0f, 0.0f, 1920.0f, 1280.0f}, (Rectangle){-offMapSize - borderWidth, -offMapSize - borderWidth, mapWidth + theExtra, mapHeight + theExtra}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f, WHITE);
    DrawTextureTiled(fgTexture, (Rectangle){0.0f, 0.0f, 516.0f, 516.0f}, (Rectangle){0.0f, 0.0f, mapWidth, mapHeight}, (Vector2){0.0f, 0.0f}, 0.0f, 1.6f, WHITE);

    // Borders
    DrawRectangle(-borderWidth, -borderWidth, mapWidth + borderWidth, borderWidth, DARKBROWN);    //TOP
    DrawRectangle(-borderWidth, 0, borderWidth, mapHeight, DARKBROWN);         //LEFT
    DrawRectangle(-borderWidth, mapHeight, mapWidth + borderWidth, borderWidth, DARKBROWN); //BOTTOM
    DrawRectangle(mapWidth, -borderWidth, borderWidth, mapHeight + borderWidth * 2, DARKBROWN);   //RIGHT
    
    // Draw fruit to pick
    for (int i = 0; i < FOOD_ITEMS; i++)
    DrawCircleV(fruits[i].position, fruits[i].size, fruits[i].color);
}

void UnloadMap(void)
{
    UnloadTexture(bgTexture);
    UnloadTexture(fgTexture);
}

void UpdateCameraCenterInsideMap(Camera2D *camera, Snake *snake, int screenWidth, int screenHeight)
{
    camera->target = snake[0].position;
    camera->offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    float minX = -borderWidth - offMapSize;
    float minY = -borderWidth - offMapSize;
    float maxX = mapWidth + borderWidth + offMapSize;
    float maxY = mapHeight + borderWidth + offMapSize;

    Vector2 max = GetWorldToScreen2D((Vector2){ maxX, maxY }, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){ minX, minY }, *camera);
    
    if (max.x < screenWidth) camera->offset.x = screenWidth - (max.x - screenWidth/2.0f);
    if (max.y < screenHeight) camera->offset.y = screenHeight - (max.y - screenHeight/2.0f);
    if (min.x > 0) camera->offset.x = screenWidth/2.0f - min.x;
    if (min.y > 0) camera->offset.y = screenHeight/2.0f - min.y;

    if (camera->zoom > 1.4f) camera->zoom = 1.4f;
    if (camera->zoom < .7f) camera->zoom = .7f;
}