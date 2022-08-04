#include "include/raylib.h"
#include "mapObjects.h"
#include <stdlib.h>
#include <sys/types.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (global)
//----------------------------------------------------------------------------------
static const u_int mapSize = 500;
const float tileSize = 512.0f;
const int mapWidth = mapSize * tileSize;
const int mapHeight = mapSize * tileSize;
int borderWidth = 40;
int offMapSize = 110; //how many pixels to fit outside the map in the screen when near borders

Food fruits[FOOD_ITEMS] = { 0 };

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static Texture2D texPalette[7] = { 0 };
static Color* colors = { 0 };
static Texture2D bgTexture = { 0 };
static Texture2D wallTexture = { 0 };
static Texture2D raspberryTexture = { 0 };
static Texture2D pineapleTexture = { 0 };
static Texture2D sushiTexture = { 0 };
static Texture2D pizzaTexture = { 0 };


//Map dimensions
static unsigned char** tileMapCoordinates = { 0 };
static unsigned short xPreLoadTile = 2;     // how many tiles to render in each axis from player
static unsigned short yPreLoadTile = 2;
static int theExtra = 0;    // extra space needed for drawing bg and fg

//Map objects
static float minusFoodLifetime = 8.0f;
static float bonusFoodLifetime = 10.0f;
static float regularFoodLifetime = 40.0f;
static int minusFruitPoints = 50;
static int bonusFruitPoints = 10;
static int regularFruitPoints = 2;
static float minusFruitScale = .8f;
static float bonusFruitScale = 1.2f;
static float regularFruitScale = .6f;
static int bonusFruitTailIncrease = 5;
static int regularFruitTailIncrease = 1;

//----------------------------------------------------------------------------------
// Map related Functions Definition
//----------------------------------------------------------------------------------
void InitMap(void)
{
    for (u_short i = 0; i < FOOD_ITEMS; i++) fruits[i].active = false;

    colors = LoadImageColors(LoadImage("../resources/textures/BWMap.png"));
    tileMapCoordinates = AssignColors(colors);

    texPalette[WATER] = LoadTexture("../resources/textures/03_Water.png");
    texPalette[SAND] = LoadTexture("../resources/textures/23_Sand.png");
    texPalette[ROCK] = LoadTexture("../resources/textures/04_Ground.png");
    texPalette[DIRT] = LoadTexture("../resources/textures/10_Dirt.png");
    texPalette[GRASS1] = LoadTexture("../resources/textures/15_Grass.png");
    texPalette[GRASS2] = LoadTexture("../resources/textures/18_Grass.png");
    texPalette[GRASS3] = LoadTexture("../resources/textures/20_Grass.png");

    bgTexture = LoadTexture("../resources/textures/04Dirt1920x1080.png");
    wallTexture = LoadTexture("../resources//textures/stone480.png");
    
    raspberryTexture = LoadTexture("../resources/items/raspberry64.png");
    pineapleTexture = LoadTexture("../resources/items/pineaple64.png");
    sushiTexture = LoadTexture("../resources/items/sushi64.png");
    pizzaTexture = LoadTexture("../resources/items/pizza64.png");

    theExtra = borderWidth * 2 + offMapSize * 2;
}

void CalcFruitPos(void)
{
    for (u_short i = 0; i < FOOD_ITEMS; i++)
    {
        if (fruits[i].lifetime <= 0) fruits[i].active = false;
        if (!fruits[i].active)
        {
            fruits[i].active = true;
            int randomValue = GetRandomValue(1, 40);
            //MinusFruit
            if (randomValue % 20 == 0)
            {
                fruits[i].scale = minusFruitScale;
                fruits[i].foodTexture = &pizzaTexture;
                fruits[i].foodType = TAILCUT;
                fruits[i].position = (Vector2){ GetRandomValue(64, mapWidth - 64), GetRandomValue(64, (mapHeight - 64) - 2)};
                fruits[i].points = minusFruitPoints;
                fruits[i].tailIncreaseSize = -counterTail/2;
                fruits[i].lifetime = minusFoodLifetime;
            }
            //Speed boost fruit
            else if (randomValue % 10 == 0) 
            {
                fruits[i].scale = .5f;
                fruits[i].foodTexture = &sushiTexture;
                fruits[i].foodType = BOOST;
                fruits[i].position = (Vector2){ GetRandomValue(64, mapWidth - 64), GetRandomValue(64, (mapHeight - 64) - 2)};
                fruits[i].points = bonusFruitPoints;
                fruits[i].tailIncreaseSize = bonusFruitTailIncrease + 5;
                fruits[i].lifetime = bonusFoodLifetime;
            }
            //Bonus fruit
            else if (randomValue % 5 == 0) 
            {
                fruits[i].scale = bonusFruitScale;
                fruits[i].foodType = BOOST;
                fruits[i].foodTexture = &pineapleTexture;
                fruits[i].position = (Vector2){ GetRandomValue(64, mapWidth - 64), GetRandomValue(64, (mapHeight - 64) - 2)};
                fruits[i].points = bonusFruitPoints;
                fruits[i].tailIncreaseSize = bonusFruitTailIncrease;
                fruits[i].lifetime = bonusFoodLifetime;
            }
            //Main fruit
            else
            {
                fruits[i].scale = regularFruitScale;
                fruits[i].foodTexture = &raspberryTexture;
                fruits[i].foodType = REGULAR;
                fruits[i].position = (Vector2){ GetRandomValue(64, mapWidth - 64), GetRandomValue(64, (mapHeight - 64) - 2)};
                fruits[i].points = regularFruitPoints;
                fruits[i].tailIncreaseSize = regularFruitTailIncrease;
                fruits[i].lifetime = regularFoodLifetime + regularFoodLifetime * GetRandomValue(-10, 10) / 20;
            }
            
            if (FruitIsOnSnake(fruits[i]))
            fruits[i].position = (Vector2){ GetRandomValue(64, mapWidth - 64), GetRandomValue(64, (mapHeight - 64) - 2)};
            
        }
        fruits[i].lifetime -= GetFrameTime();
    }
}

void DrawMap(void)
{
    // BG and FG
    if (snake->tileXPos <= 1 || snake->tileXPos >= mapSize - 2 || snake->tileYPos <= 1 || snake->tileYPos >= mapSize - 2)
    DrawTextureTiled(bgTexture, (Rectangle){0.0f, 0.0f, 1920.0f, 1280.0f}, (Rectangle){-offMapSize - borderWidth, -offMapSize - borderWidth, mapWidth + theExtra, mapHeight + theExtra}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f, WHITE);

    //Clamp iterators to 0 or MAP
    for (u_short i = MAX((snake->tileYPos - yPreLoadTile), 0); i < MIN((snake->tileYPos + yPreLoadTile + 1), mapSize); i++)
    {
        for (u_short j = MAX((snake->tileXPos - xPreLoadTile), 0); j < MIN((snake->tileXPos + xPreLoadTile + 1), mapSize); j++)
        {
            DrawTexture(texPalette[tileMapCoordinates[i][j]], j * tileSize, i * tileSize, WHITE);
            DrawText(TextFormat("[ %d : %d ]", j, i), j * tileSize + tileSize / 2 - (float)MeasureText(TextFormat("[ %d : %d ]", j, i), 46) / 2, i * tileSize + tileSize / 2, 46, BLACK);
            DrawText(TextFormat("[%d : %d : %d]", colors[i * mapSize + j].r, colors[i * mapSize + j].g, colors[i * mapSize + j].b),
                    j * tileSize + tileSize / 2 - (float)MeasureText(TextFormat("[%d : %d : %d]", colors[i * mapSize + j].r, colors[i * mapSize + j].g, colors[i * mapSize + j].b), 38) / 2,
                    i * tileSize + tileSize / 1.5,
                    38, PURPLE);
        }
    }

    // Borders
    DrawTextureTiled(wallTexture, (Rectangle){0.0f, 0.0f, 480.0f, 480.0f}, (Rectangle){-borderWidth, -borderWidth, mapWidth + borderWidth, borderWidth}, (Vector2){0.0f, 0.0f}, 0.0f, .5f, WHITE);
    DrawTextureTiled(wallTexture, (Rectangle){0.0f, 0.0f, 480.0f, 480.0f}, (Rectangle){-borderWidth, 0, borderWidth, mapHeight}, (Vector2){0.0f, 0.0f}, 0.0f, .5f, WHITE);
    DrawTextureTiled(wallTexture, (Rectangle){0.0f, 0.0f, 480.0f, 480.0f}, (Rectangle){-borderWidth, mapHeight, mapWidth + borderWidth, borderWidth}, (Vector2){0.0f, 0.0f}, 0.0f, .5f, WHITE);
    DrawTextureTiled(wallTexture, (Rectangle){0.0f, 0.0f, 480.0f, 480.0f}, (Rectangle){mapWidth, -borderWidth, borderWidth, mapHeight + borderWidth * 2}, (Vector2){0.0f, 0.0f}, 0.0f, .5f, WHITE);
    
    // Draw fruit to pick
    for (u_short i = 0; i < FOOD_ITEMS; i++)
    {
        DrawTextureEx(*fruits[i].foodTexture, (Vector2){fruits[i].position.x - 32 * fruits[i].scale, fruits[i].position.y - 32 * fruits[i].scale}, 0, fruits[i].scale, WHITE);
        DrawCircleLines(fruits[i].position.x, fruits[i].position.y, 32 * fruits[i].scale, RED);
    }
}

void UpdateCameraCenterInsideMap(Camera2D *camera, int screenWidth, int screenHeight)
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

unsigned char** AssignColors(Color* colors)
{
    unsigned char** collArray = (unsigned char**) RL_MALLOC(mapSize * sizeof(unsigned char*));
    for (u_short i = 0; i < mapSize; i++) collArray[i] = (unsigned char*) RL_MALLOC(mapSize * sizeof(unsigned char));

    for (u_short y = 0; y < mapSize; y++)
    {
        for (u_short x = 0; x < mapSize; x++)
        {
            if (colors[y * mapSize + x].b >= 220 &&
                colors[y * mapSize + x].r <= 220 &&
                colors[y * mapSize + x].g <= 220) collArray[y][x] = WATER;

            if (colors[y * mapSize + x].r <= 10) collArray[y][x] = SAND;
            else if (colors[y * mapSize + x].r <= 50) collArray[y][x] = DIRT;
            else if (colors[y * mapSize + x].r <= 90) collArray[y][x] = GRASS1;
            else if (colors[y * mapSize + x].r <= 130) collArray[y][x] = GRASS2;
            else if (colors[y * mapSize + x].r <= 170) collArray[y][x] = GRASS3;
            else collArray[y][x] = ROCK;
        }
    }
    return collArray;
}

void UnloadMap(void)
{
    UnloadTexture(bgTexture);
    for (u_short i = 0; i < 6; i++) UnloadTexture(texPalette[i]);
    UnloadTexture(wallTexture);
    UnloadTexture(raspberryTexture);
    UnloadTexture(pineapleTexture);
    UnloadTexture(sushiTexture);
    UnloadTexture(pizzaTexture);
    RL_FREE(tileMapCoordinates);
}
