#include "raylib.h"
#include "mapObjects.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static Texture2D bgTexture = { 0 };
static Texture2D fgTexture = { 0 };
static Food fruits[FOOD_ITEMS] = { 0 };

static float bonusFoodLifetime = 10.0f;
static float regularFoodLifetime = 40.0f;
static int bonusFruitPoints = 10;
static int regularFruitPoints = 2;
static float bonusFruitSize = 20.0f;
static float regularFruitSize = 12.0f;
static int theExtra = 0;    // extra space needed for drawing bg and fg

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

void CalcFruitPos()
{
    for (int i = 0; i < FOOD_ITEMS; i++)
    {
        if (fruits[i].lifetime <= 0) fruits[i].active = false;
        if (!fruits[i].active)
        {
            fruits[i].active = true;
            int randomValue = GetRandomValue(1, 20);
            //Bonus fruit
            if (randomValue % 10 == 0) 
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
            
            if (FruitIsOnSnake(fruits[i]))
            fruits[i].position = (Vector2){ GetRandomValue(fruits[i].size, mapWidth - fruits[i].size), GetRandomValue(fruits[i].size, (mapHeight - fruits[i].size) - 2)};
            
        }
        fruits[i].lifetime -= GetFrameTime();
    }
}

Food* GetFoodObject(int i)
{
    return fruits + i;
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
