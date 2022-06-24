#include "raylib.h"
#include "mapObjects.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static Texture2D bgTexture = { 0 };
static Texture2D fgTexture = { 0 };
static Food fruits[FOOD_ITEMS] = { 0 };

const int mapHeight = 60;    //squares
const int mapWidth = 100;
static float bonusFoodLifetime = 10.0f; //don't touch thing for a timer fucntion
static int offMapSize = 5; //how many blocks to fit outside the map in the screen when near borders

//----------------------------------------------------------------------------------
// Map related Functions Definition
//----------------------------------------------------------------------------------
void InitMap(void)
{
    for (int i = 0; i < FOOD_ITEMS; i++) fruits[i].active = false;

    bgTexture = LoadTexture("../resources/dirtSIZE.png");
    fgTexture = LoadTexture("../resources/grass516.png");
}

void CalcFruitPos(Snake snake[], int* counterTail)
{
    for (int i = 0; i < FOOD_ITEMS; i++)
    {
        if (fruits[i].lifetime <= 0) fruits[i].active = false;
        if (!fruits[i].active)
        {
            fruits[i].active = true;
            fruits[i].position = (Vector2){ GetRandomValue(1, mapWidth - 2)*SQUARE_SIZE, GetRandomValue(1, (mapHeight) - 2)*SQUARE_SIZE};
            //Bonus fruit
            if (GetRandomValue(1, 20) % 5 == 0) 
            {
                fruits[i].points = 10;
                fruits[i].color = MAROON;
                fruits[i].size = (Vector2){SQUARE_SIZE * 2, SQUARE_SIZE * 2};
                fruits[i].lifetime = bonusFoodLifetime;
            }

            //Main fruit
            else
            {
                fruits[i].points = 1;
                fruits[i].color = YELLOW;
                fruits[i].size = (Vector2){SQUARE_SIZE, SQUARE_SIZE};
                fruits[i].lifetime = 40.0f;
            }

            for (int i = 0; i < *counterTail; i++)   //To prevent a fruit from spawning on top of a snake
            {
                while ((fruits[i].position.x == snake[i].position.x) && (fruits[i].position.y == snake[i].position.y))
                {
                    fruits[i].position = (Vector2){ GetRandomValue(1, mapWidth - 2)*SQUARE_SIZE, GetRandomValue(1, mapHeight - 2)*SQUARE_SIZE};
                    i = 0;
                }
            }
        }
        fruits[i].lifetime -= GetFrameTime();
    }
}

bool CalcWallCollision(Snake snake[])
{
    return (((snake[0].position.x) > (mapWidth * SQUARE_SIZE - SQUARE_SIZE * 2)) ||
            ((snake[0].position.y) > (mapHeight * SQUARE_SIZE - SQUARE_SIZE * 2)) ||
            (snake[0].position.x < SQUARE_SIZE) || (snake[0].position.y < SQUARE_SIZE));
}

void CalcFruitCollision(Snake snake[], int* counterTail, Vector2 snakePosition[], int* score)
{
    for (int i = 0; i < FOOD_ITEMS; i++)
    {
        if ((snake[0].position.x < (fruits[i].position.x + fruits[i].size.x) && (snake[0].position.x + snake[0].size.x) > fruits[i].position.x) &&
            (snake[0].position.y < (fruits[i].position.y + fruits[i].size.y) && (snake[0].position.y + snake[0].size.y) > fruits[i].position.y))
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
    DrawTextureTiled(bgTexture, (Rectangle){0.0f, 0.0f, 1920.0f, 1280.0f}, (Rectangle){-offMapSize * SQUARE_SIZE, -offMapSize * SQUARE_SIZE, 3000.0f, 2000.0f}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f, WHITE);
    DrawTextureTiled(fgTexture, (Rectangle){0.0f, 0.0f, 516.0f, 516.0f}, (Rectangle){0.0f, 0.0f, mapWidth * SQUARE_SIZE, mapHeight * SQUARE_SIZE}, (Vector2){0.0f, 0.0f}, 0.0f, 1.6f, WHITE);

    // Borders
    DrawRectangle(0, 0, SQUARE_SIZE, mapHeight * SQUARE_SIZE, DARKBROWN);
    DrawRectangle(SQUARE_SIZE, 0, mapWidth * SQUARE_SIZE - SQUARE_SIZE, SQUARE_SIZE, DARKBROWN);
    DrawRectangle(SQUARE_SIZE, mapHeight * SQUARE_SIZE - SQUARE_SIZE, mapWidth * SQUARE_SIZE - SQUARE_SIZE, SQUARE_SIZE, DARKBROWN);
    DrawRectangle(mapWidth * SQUARE_SIZE - SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, mapHeight * SQUARE_SIZE - SQUARE_SIZE * 2, DARKBROWN);
    
    // Draw fruit to pick
    for (int i = 0; i < FOOD_ITEMS; i++)
    DrawCircleV((Vector2){fruits[i].position.x + fruits[i].size.x / 2, fruits[i].position.y + fruits[i].size.x / 2}, fruits[i].size.x / 2, fruits[i].color);
}

void UnloadMap(void)
{
    UnloadTexture(bgTexture);
    UnloadTexture(fgTexture);
}

void UpdateCameraCenterInsideMap(Camera2D *camera, Snake *player, int screenWidth, int screenHeight)
{
    camera->target = player[0].position;
    camera->offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    float minX = -SQUARE_SIZE * offMapSize;
    float minY = -SQUARE_SIZE * offMapSize;
    float maxX = (mapWidth + offMapSize) * SQUARE_SIZE;
    float maxY = (mapHeight + offMapSize) * SQUARE_SIZE;

    Vector2 max = GetWorldToScreen2D((Vector2){ maxX, maxY }, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){ minX, minY }, *camera);
    
    if (max.x < screenWidth) camera->offset.x = screenWidth - (max.x - screenWidth/2.0f);
    if (max.y < screenHeight) camera->offset.y = screenHeight - (max.y - screenHeight/2.0f);
    if (min.x > 0) camera->offset.x = screenWidth/2.0f - min.x;
    if (min.y > 0) camera->offset.y = screenHeight/2.0f - min.y;

    if (camera->zoom > 1.5f) camera->zoom = 1.5f;
    if (camera->zoom < .6f) camera->zoom = .6f;
}