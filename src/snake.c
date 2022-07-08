#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mapObjects.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static Snake snake[SNAKE_LENGTH] = { 0 };
static Vector2 snakePosition[SNAKE_LENGTH] = { 0 };
static Color SnakeColorPatern1[] = { ORANGE, SKYBLUE, MAGENTA, LIME };
//Aceleration
static Vector2 currentSpeed = { 0 };
static bool accelerating;

static float snakeSizeRadius = 20;
int counterTail = 0;
int snakeSpeed = 3;
//Global variables
int score = 0;
float snakeSpeedX = 0, snakeSpeedY = 0;

//Controlls
//Keys
float turnAngle = 8.0f;
float cosAnglePositive = 0;
float sinAnglePositive = 0;
float cosAngleNegative = 0;
float sinAngleNegative = 0;
//Mouse
// Vector2 screnPos = { 0 };
// Vector2 mousePos = { 0 };
// float angleMouse = 0.0f;
// float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;

//----------------------------------------------------------------------------------
// Map Functions Definition
//----------------------------------------------------------------------------------
void InitSnake(void)
{
    //Needed for keyboard movement
    cosAnglePositive = cosf(turnAngle * DEG2RAD);
    sinAnglePositive = sinf(turnAngle * DEG2RAD);
    cosAngleNegative = cosf(-turnAngle * DEG2RAD);
    sinAngleNegative = sinf(-turnAngle * DEG2RAD);

    counterTail = 2;
    score = counterTail - 2;
    accelerating = false;

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){ 400.0f, 400.0f };
        snake[i].size = snakeSizeRadius;
        snake[i].speed = (Vector2){ snakeSpeed, snakeSpeed };

        //5 and 4 are hardcoded 5 - number of circles; 4 - number of elements in array
        snake[i].color = SnakeColorPatern1[i / 5 % 4];     //every 5 circles are different colors
    }

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    }
}

void SetSnakeAsCameraTarget(Camera2D *camera)
{
    camera->target = snake[0].position;
}

void UpdateMovement(Camera2D *camera)
{   
    // mousePos = GetMousePosition();

    //     /*Mouse controlls*/
    // screnPos = GetScreenToWorld2D(mousePos, *camera);
    // dx = screnPos.x - snake[0].position.x;
    // dy = screnPos.y - snake[0].position.y;
    // angleMouse = atan2f(dy, dx);       //cos = x(-1;1)     sin = y(-1;1)

    // dxx = cosf(angleMouse);
    // dyy = sinf(angleMouse);

    // //Sets the direction. If top/down/left/right, then move straight
    // if (dxx >= .99f) snake[0].speed = (Vector2){snakeSpeed * 2, 0};
    // else if (dxx <= -.99f) snake[0].speed = (Vector2){-snakeSpeed * 2, 0};
    // else if (dyy >= .99f) snake[0].speed = (Vector2){0, snakeSpeed * 2};
    // else if (dyy <= -.99f) snake[0].speed = (Vector2){0, -snakeSpeed * 2};
    // else if (dxx > 0 && dyy > 0) snake[0].speed = (Vector2){snakeSpeed + snakeSpeed * dxx, snakeSpeed + snakeSpeed * dyy};
    // else if (dxx > 0 && dyy < 0) snake[0].speed = (Vector2){snakeSpeed + snakeSpeed * dxx, -snakeSpeed + snakeSpeed * dyy};
    // else if (dxx < 0 && dyy > 0) snake[0].speed = (Vector2){-snakeSpeed + snakeSpeed * dxx, snakeSpeed + snakeSpeed * dyy};
    // else snake[0].speed = (Vector2){-snakeSpeed + snakeSpeed * dxx, -snakeSpeed + snakeSpeed * dyy};

    if (!accelerating) currentSpeed = snake[0].speed;

        /*Keyboard controlls*/
    if (IsKeyDown(KEY_RIGHT) && !accelerating)
    {
        snake[0].speed = (Vector2){snake[0].speed.x * cosAnglePositive - snake[0].speed.y * sinAnglePositive, snake[0].speed.x * sinAnglePositive + snake[0].speed.y * cosAnglePositive};
    }
    else if (IsKeyDown(KEY_LEFT) && !accelerating)
    {
        snake[0].speed = (Vector2){snake[0].speed.x * cosAngleNegative - snake[0].speed.y * sinAngleNegative, snake[0].speed.x * sinAngleNegative + snake[0].speed.y * cosAngleNegative};
    }
    snakeSpeedX = snake[0].speed.x;
    snakeSpeedY = snake[0].speed.y;

    //Camera zoom
    if (IsKeyDown(KEY_Q)) camera->zoom += .01f;
    if (IsKeyDown(KEY_E)) camera->zoom -= .01f;
    
    //Acceleration
    if (IsKeyPressed(KEY_SPACE))
    {
        accelerating = true;
        snake[0].speed = (Vector2){snake[0].speed.x * 2.0f , snake[0].speed.y * 2.0f};
    }
    if (IsKeyReleased(KEY_SPACE))
    {
        accelerating = false;
        snake[0].speed = currentSpeed;
    }
}

bool CalcWallCollision()
{
    return (((snake[0].position.x - snake[0].size) > (mapWidth - borderWidth)) ||
            ((snake[0].position.y - snake[0].size) > (mapHeight - borderWidth)) ||
            (snake[0].position.x - snake[0].size < 0) || (snake[0].position.y - snake[0].size < 0));
}

bool CalcSelfCollision(void)        //TODO work on better collision recognition
{
    for (int i = 1; i < counterTail; i++)
    {
        return ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y));
    }
    return false;
}

void MoveSnake(void)
{
    for (int i = 0; i < counterTail; i++)
    {
        snakePosition[i] = snake[i].position;

        if (i == 0)
        {
            snake[0].position.x += snake[0].speed.x;
            snake[0].position.y += snake[0].speed.y;
        }
        else snake[i].position = snakePosition[i-1];
    }
}

void DrawSnake()
{
    for (int i = counterTail - 1; i > 0; i--) DrawCircleV(snake[i].position, snake[i].size, snake[i].color);
}

bool FruitIsOnSnake(Food fruit)
{
    for (int i = 0; i < counterTail; i++)   //To prevent a fruit from spawning on top of a snake
    {
        return ((fruit.position.x == snake[i].position.x) && (fruit.position.y == snake[i].position.y));
    }
    return false;
}

void CalcFruitCollision(void)
{
    for (int i = 0; i < FOOD_ITEMS; i++)
    {
        if (CheckCollisionCircles(snake[0].position, snake[0].size, fruits[i].position, 32 * fruits[i].scale))
        {
            if (fruits[i].tailIncreaseSize >= 0)
            {
                for (short j = 0; j < fruits[i].tailIncreaseSize; j++)
                snake[counterTail + j].position = snakePosition[counterTail - 1];
            }
            else
            {
                for (short j = 0; j < abs(fruits[i].tailIncreaseSize); j++)
                snake[counterTail - j].position = snakePosition[counterTail - 1];

            }
            counterTail += fruits[i].tailIncreaseSize;
            score += fruits[i].points;
            fruits[i].active = false;
        }
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