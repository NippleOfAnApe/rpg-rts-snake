#include "include/raylib.h"
#include "include/raymath.h"
#include <stdbool.h>
#include <stdlib.h>
#include "mapObjects.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (global)
//----------------------------------------------------------------------------------
Snake snake[SNAKE_LENGTH] = { 0 };
int score = 0;
int counterTail = 0;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static Vector2 snakePosition[SNAKE_LENGTH] = { 0 };
static Color SnakeColorPatern1[] = { ORANGE, SKYBLUE, MAGENTA, LIME, YELLOW };
//Aceleration
static Vector2 currentSpeed = { 0 };
static bool accelerating;
static float snakeSizeRadius = 20;
static short snakeColorFrequency = 8;   //how many circles are of the same color
static int snakeSpeed = 3;
static int tailStartSize = 8;

//Controlls
//Keys
static float turnAngle = 6.0f;
static float cosAnglePositive = 0;
static float sinAnglePositive = 0;
static float cosAngleNegative = 0;
static float sinAngleNegative = 0;
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

    counterTail = tailStartSize;
    score = counterTail - tailStartSize;
    accelerating = false;

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){ 100.0f, 100.0f };
        snake[i].size = snakeSizeRadius;
        snake[i].speed = (Vector2){ snakeSpeed, snakeSpeed };
        snake[i].color = SnakeColorPatern1[i / snakeColorFrequency % (sizeof(SnakeColorPatern1) / sizeof(Color))];     //every 5 circles are different colors
        
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    }

    snake->boostCapacity = 0.0f;
    snake->tileXPos = 0;
    snake->tileYPos = 0;
}

void SetSnakeAsCameraTarget(Camera2D *camera)
{
    camera->target = snake->position;
}

void UpdateMovement(Camera2D *camera)
{   
    // mousePos = GetMousePosition();

    //     /*Mouse controlls*/
    // screnPos = GetScreenToWorld2D(mousePos, *camera);
    // dx = screnPos.x - snake->position.x;
    // dy = screnPos.y - snake->position.y;
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

    snake->tileXPos = snake->position.x / tileSize;
    snake->tileYPos = snake->position.y / tileSize;

    if (!accelerating) currentSpeed = snake->speed;

        /*Keyboard controlls*/
    if (IsKeyDown(KEY_RIGHT) && IsKeyUp(KEY_LEFT) && !accelerating)
    {
        snake->speed = (Vector2){snake->speed.x * cosAnglePositive - snake->speed.y * sinAnglePositive, snake->speed.x * sinAnglePositive + snake->speed.y * cosAnglePositive};
    }
    else if (IsKeyDown(KEY_LEFT) && IsKeyUp(KEY_RIGHT) && !accelerating)
    {
        snake->speed = (Vector2){snake->speed.x * cosAngleNegative - snake->speed.y * sinAngleNegative, snake->speed.x * sinAngleNegative + snake->speed.y * cosAngleNegative};
    }

    //Camera zoom
    if (IsKeyDown(KEY_Q)) camera->zoom += .01f;
    if (IsKeyDown(KEY_E)) camera->zoom -= .01f;
    
    //Acceleration
    if (IsKeyDown(KEY_SPACE))
    {
        // if (snake->boostCapacity > 0)
        // {
        //     accelerating = true;
        //     snake->boostCapacity -= .1f;
        //     snake->speed = (Vector2){currentSpeed.x * 2.0f, currentSpeed.y * 2.0f};
        // }else
        // {
        //     snake->boostCapacity = 0.0f;
        //     accelerating = false;
        //     snake->speed = currentSpeed;
        // }
        accelerating = true;
        snake->speed = (Vector2){currentSpeed.x * 8.0f, currentSpeed.y * 8.0f};
    }

    if (IsKeyReleased(KEY_SPACE))
    {
        accelerating = false;
        snake->speed = currentSpeed;
    }
}

bool CalcWallCollision()
{
    return (((snake->position.x - snake->size) > (mapWidth - borderWidth)) ||
            ((snake->position.y - snake->size) > (mapHeight - borderWidth)) ||
            (snake->position.x - snake->size < 0) || (snake->position.y - snake->size < 0));
}

bool CalcSelfCollision(void)        //TODO work on better collision recognition
{
    // for (int i = 1; i < counterTail; i++)
    // {
    //     return ((snake->position.x == snake[i].position.x) && (snake->position.y == snake[i].position.y));
    // }
    return false;
}

void MoveSnake(void)
{
    for (int i = 0; i < counterTail; i++)
    {
        snakePosition[i] = snake[i].position;

        if (i == 0)
        {
            snake->position.x += snake->speed.x;
            snake->position.y += snake->speed.y;
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
        return CheckCollisionCircles(fruit.position, fruit.scale, snake[i].position, snake[i].size);
    }
    return false;
}

void CalcFruitCollision(void)
{
    for (int i = 0; i < FOOD_ITEMS; i++)
    {
        if (CheckCollisionCircles(snake->position, snake->size, fruits[i].position, 32 * fruits[i].scale))
        {

            if (fruits[i].foodType == TAILCUT && counterTail > 2)
            {
                for (short j = 0; j < abs(fruits[i].tailIncreaseSize); j++)
                snake[counterTail - j].position = snakePosition[counterTail - 1];
            }
            else
            {
                for (short j = 0; j < fruits[i].tailIncreaseSize; j++)
                snake[counterTail + j].position = snakePosition[counterTail - 1];
            }
            
            if (fruits[i].foodType == BOOST)
            {
                snake->boostCapacity += 40;
            }
            counterTail += fruits[i].tailIncreaseSize;
            score += fruits[i].points;
            fruits[i].active = false;

            //Increase circle size
            for (int i = 0; i < SNAKE_LENGTH; i++)
            {
                snake[i].size = snakeSizeRadius + counterTail * .05f;
            }

            if (counterTail < 50) turnAngle = 8.0f;
            else if (counterTail < 100) turnAngle = 7.0f;
            else if (counterTail < 150) turnAngle = 6.0f;
            else if (counterTail < 200) turnAngle = 5.0f;
            else if (counterTail < 250) turnAngle = 4.0f;
            else if (counterTail < 300) turnAngle = 3.0f;
            else turnAngle = 2.0f;

            //Needed for keyboard movement
            cosAnglePositive = cosf(turnAngle * DEG2RAD);
            sinAnglePositive = sinf(turnAngle * DEG2RAD);
            cosAngleNegative = cosf(-turnAngle * DEG2RAD);
            sinAngleNegative = sinf(-turnAngle * DEG2RAD);
        }
    }
}
