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

static Camera2D camera = { 0 };
//static bool allowMove = false;
//int speed = 3;  //5, 3 or 2; because %60    FRAME


//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)
static void DrawUI(void);
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

    //allowMove = false;

    SetSnakeAsCameraTarget(&camera);
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    InitSnake();
    InitMap();
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}

// Update game (one frame)`LOGIC
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            // Player controls
            UpdateMovement(&camera);

            // Snake movement
            MoveSnake();

            // Wall collision or Collision with self
            gameOver = CalcWallCollision() || CalcSelfCollision();

            // Fruit position calculation
            CalcFruitPos();

            // Collision
            CalcFruitCollision();
            
            //Camera updater
            UpdateCameraCenterInsideMap(&camera, screenWidth, screenHeight);

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
void DrawGame(void)
{
    BeginDrawing();

        ClearBackground(GRAY);
        if (!gameOver)
        {
            BeginMode2D(camera);
            //DrawGridUI();
            DrawMap();

            // Draw snake
            DrawSnake();
        
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
    DrawText(TextFormat("TailCount: %d / %d", counterTail, SNAKE_LENGTH), 30, 400, 24, WHITE);
    DrawText(TextFormat("snake[0].speed.x: %.02f", snakeSpeedX), 30, 60, 28, DARKPURPLE);
    DrawText(TextFormat("snake speed y: %.02f", snakeSpeedY), 30, 100, 28, DARKPURPLE);
}
