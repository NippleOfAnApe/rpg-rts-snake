/*******************************************************************************************
*
*   raylib - classic game: snake
*
*   Sample game developed by Ian Eito, Albert Martos and Ramon Santamaria
*
*   This game has been created using raylib v1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "mapObjects.h"
#include <stdbool.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 1000;

static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;

static Snake snake[SNAKE_LENGTH] = { 0 };
static Vector2 snakePosition[SNAKE_LENGTH] = { 0 };
static Color SnakeColorPatern1[] = { ORANGE, SKYBLUE, MAGENTA, LIME };

static float snakeSizeRadius = 20;
static Camera2D camera = { 0 };
//static bool allowMove = false;
static int counterTail = 0;
int score = 0;
int snakeSpeed = 3;    //FREQUENCY
//int speed = 3;  //5, 3 or 2; because %60    FRAME

Vector2 screnPos = { 0 };
Vector2 mousePos = { 0 };
float angleMouse = 0.0f, angleSnake = 0.0f;
float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;
float dxSnake = 0.0f, dySnake = 0.0f, dxxSnake = 0.0f, dyySnake = 0.0f;


//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)
static void DrawUI(void);
static void UpdateMovement(void);
static void CollisionWithYourself(void);
//static Color AssignSnakeColors(int i);  // recursive color assigning

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Make the window resizable
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "My Snake");
    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        mousePos = GetMousePosition();
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void)
{
    framesCounter = 0;
    gameOver = false;
    pause = false;

    counterTail = 2;
    score = counterTail - 2;
    //allowMove = false;

    camera.target = snake[0].position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
        snake[i].size = snakeSizeRadius;
        snake[i].speed = (Vector2){ 0, 0 };

        
        if (i == 0) snake[i].color = DARKBLUE;
        //5 and for are hardcoded
        else snake[i].color = SnakeColorPatern1[i / 5 % 4];     //every 5 circles are different colors
    }

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    }

    InitMap();
}

// Update and Draw (one frame)
void UpdateDrawFrame()
{
    UpdateGame();
    DrawGame();
}

// Update game (one frame)`LOGIC
void UpdateGame()
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            // Player controls
            UpdateMovement();

            // Snake movement

            for (int i = 0; i < counterTail; i++)
            {
                snakePosition[i] = snake[i].position;

                if (i == 0)
                {
                    snake[0].position.x += snake[0].speed.x;
                    snake[0].position.y += snake[0].speed.y;
                    //allowMove = true;
                }
                else snake[i].position = snakePosition[i-1];
            }

            // Wall collision
            gameOver = CalcWallCollision(snake);

            // Collision with yourself
            CollisionWithYourself();

            // Fruit position calculation
            CalcFruitPos(snake, &counterTail);

            // Collision
            CalcFruitCollision(snake, &counterTail, snakePosition, &score);
            
            //Camera updater
            UpdateCameraCenterInsideMap(&camera, snake, screenWidth, screenHeight);

            framesCounter++;
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame()
{
    BeginDrawing();

        ClearBackground(GRAY);
        if (!gameOver)
        {
            BeginMode2D(camera);
            //DrawGridUI();
            DrawMap();

            // Draw snake
            for (int i = counterTail - 1; i > 0; i--) DrawCircleV(snake[i].position, snake[i].size, snake[i].color);
        
            EndMode2D();
            DrawUI();   //UI on top of game elements
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, RAYWHITE);

    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
    UnloadMap();
}

// Draws UI textboxes
void DrawUI(void)
{
    if (!pause)
    DrawText("Press P to pause", screenWidth - MeasureText("Press P to pause", 18) - 20, 20, 18, BLACK);
    else
    {
        DrawText("Press P to continue", screenWidth - MeasureText("Press P to continue", 18) - 20, 20, 18, BLACK);
        DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
    }
    DrawText(TextFormat("SCORE: %02i", score), 30, 30, 24, MAROON);
    DrawText(TextFormat("dxx: %.02f", dxSnake), 30, 60, 28, DARKPURPLE);
    DrawText(TextFormat("dyy: %.02f", dySnake), 30, 100, 28, DARKPURPLE);
    DrawText(TextFormat("mouse angle: %.02f", angleMouse), 500, 40, 28, DARKPURPLE);
}

void UpdateMovement()
{   
    screnPos = GetScreenToWorld2D(mousePos, camera);
    dx = screnPos.x - snake[0].position.x;
    dy = screnPos.y - snake[0].position.y;
    angleMouse = atan2f(dy, dx);       //cos = x(-1;1)     sin = y(-1;1)

    dxSnake = snake[0].position.x - snake[1].position.x;
    dySnake = snake[0].position.y - snake[1].position.y;
    angleSnake = atan2f(dySnake, dxSnake);       //cos = x(-1;1)     sin = y(-1;1)

    dxx = cosf(angleMouse);
    dyy = sinf(angleMouse);
    // dxxSnake = cosf(angleSnake);
    // dyySnake = sinf(angleSnake);
    

    //Sets the direction. If top/down/left/right, then move straight
    if (dxx >= .99f) snake[0].speed = (Vector2){snakeSpeed * 2, 0};
    else if (dxx <= -.99f) snake[0].speed = (Vector2){-snakeSpeed * 2, 0};
    else if (dyy >= .99f) snake[0].speed = (Vector2){0, snakeSpeed * 2};
    else if (dyy <= -.99f) snake[0].speed = (Vector2){0, -snakeSpeed * 2};
    else if (dxx > 0 && dyy > 0) snake[0].speed = (Vector2){snakeSpeed + snakeSpeed * dxx, snakeSpeed + snakeSpeed * dyy};
    else if (dxx > 0 && dyy < 0) snake[0].speed = (Vector2){snakeSpeed + snakeSpeed * dxx, -snakeSpeed + snakeSpeed * dyy};
    else if (dxx < 0 && dyy > 0) snake[0].speed = (Vector2){-snakeSpeed + snakeSpeed * dxx, snakeSpeed + snakeSpeed * dyy};
    else snake[0].speed = (Vector2){-snakeSpeed + snakeSpeed * dxx, -snakeSpeed + snakeSpeed * dyy};


    // if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove)
    // {
    //     snake[0].speed = (Vector2){ snakeSpeed, 0 };
    //     allowMove = false;
    // }
    // if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0) && allowMove)
    // {
    //     snake[0].speed = (Vector2){ -snakeSpeed, 0 };
    //     allowMove = false;
    // }
    // if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0) && allowMove)
    // {
    //     snake[0].speed = (Vector2){ 0, -snakeSpeed };
    //     allowMove = false;
    // }
    // if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0) && allowMove)
    // {
    //     snake[0].speed = (Vector2){ 0, snakeSpeed };
    //     allowMove = false;
    // }

    if (IsKeyDown(KEY_Q)) camera.zoom += .01f;
    if (IsKeyDown(KEY_E)) camera.zoom -= .01f;
}

void CollisionWithYourself(void)
{
    for (int i = 1; i < counterTail; i++)
    {
        if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y)) gameOver = true;
    }
}