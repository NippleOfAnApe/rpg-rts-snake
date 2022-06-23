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
#include <stdbool.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SNAKE_LENGTH   256
#define SQUARE_SIZE     25
#define FOOD_ITEMS      4

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Snake {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    Vector2 size;
    bool active;
    Color color;
    int points;
    float lifetime;
} Food;

typedef struct Timer {
    float Lifetime;
} Timer;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 1000;
static const int mapHeight = 60;    //squares
static const int mapWidth = 100;
static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;

//123static Food fruit = { 0 };
static Food fruits[FOOD_ITEMS] = { 0 };
static Snake snake[SNAKE_LENGTH] = { 0 };
static Vector2 snakePosition[SNAKE_LENGTH] = { 0 };
static Timer timer = { 0 };
static bool allowMove = false;
//static Vector2 offset = { 0 };
static int counterTail = 0;
int offMapSize = 5; //how many blocks to fit outside the map in the screen when near borders
static Texture2D bgTexture = { 0 };
static Texture2D fgTexture = { 0 };
int score = 0;
int speed = 5;  //5, 3 or 2; because %60
float bonusFoodLifetime = 10.0f; //don't touch thing for a timer fucntion

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(Camera2D* camera);       // Update game (one frame)
static void DrawGame(Camera2D* camera);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(Camera2D* camera);  // Update and Draw (one frame)
void DrawUI(void);
void DrawGridUI(void);
void UpdateCameraCenterInsideMap(Camera2D *camera, Snake *player);

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
    Camera2D camera = { 0 };
    camera.target = snake[0].position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

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
        UpdateDrawFrame(&camera);
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

    counterTail = 1;
    score = counterTail - 1;
    allowMove = false;
    timer.Lifetime = 0;

    // offset.x = screenWidth%SQUARE_SIZE;
    // offset.y = screenHeight%SQUARE_SIZE;

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
        snake[i].size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
        snake[i].speed = (Vector2){ SQUARE_SIZE, 0 };

        if (i == 0) snake[i].color = DARKBLUE;
        else snake[i].color = BLUE;
    }

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    }

    for (int i = 0; i < FOOD_ITEMS; i++) fruits[i].active = false;

    bgTexture = LoadTexture("resources/dirtSIZE.png");
    fgTexture = LoadTexture("resources/grass516.png");
}

// Update game (one frame)`LOGIC
void UpdateGame(Camera2D* camera)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            // Player control
            if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ -SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, -SQUARE_SIZE };
                allowMove = false;
            }
            if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_D)) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, SQUARE_SIZE };
                allowMove = false;
            }
            if (IsKeyDown(KEY_Q)) camera->zoom += .01f;
            if (IsKeyDown(KEY_E)) camera->zoom -= .01f;

            // Snake movement
            for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].position;

            if ((framesCounter%speed) == 0)
            {
                for (int i = 0; i < counterTail; i++)
                {
                    if (i == 0)
                    {
                        snake[0].position.x += snake[0].speed.x;
                        snake[0].position.y += snake[0].speed.y;
                        allowMove = true;
                    }
                    else snake[i].position = snakePosition[i-1];
                }
            }

            // Wall collision
            if (((snake[0].position.x) > (mapWidth * SQUARE_SIZE - SQUARE_SIZE * 2)) ||
                ((snake[0].position.y) > (mapHeight * SQUARE_SIZE - SQUARE_SIZE * 2)) ||
                (snake[0].position.x < SQUARE_SIZE) || (snake[0].position.y < SQUARE_SIZE))
            {
                gameOver = true;
            }

            // Collision with yourself
            for (int i = 1; i < counterTail; i++)
            {
                if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y)) gameOver = true;
            }

            // Fruit position calculation
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

                    for (int i = 0; i < counterTail; i++)   //To prevent a fruit from spawning on top of a snake
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

            // Collision
            for (int i = 0; i < FOOD_ITEMS; i++)
            {
                if ((snake[0].position.x < (fruits[i].position.x + fruits[i].size.x) && (snake[0].position.x + snake[0].size.x) > fruits[i].position.x) &&
                    (snake[0].position.y < (fruits[i].position.y + fruits[i].size.y) && (snake[0].position.y + snake[0].size.y) > fruits[i].position.y))
                {
                    snake[counterTail].position = snakePosition[counterTail - 1];
                    counterTail += 1;
                    score += fruits[i].points;
                    fruits[i].active = false;
                }
            }
            
            UpdateCameraCenterInsideMap(camera, snake);

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
void DrawGame(Camera2D* camera)
{
    BeginDrawing();

        ClearBackground(GRAY);
        if (!gameOver)
        {
            // Draw grid lines
            //DrawTextureEx(bgTexture, (Vector2){0, 0}, 0, 2.5, WHITE);
            BeginMode2D(*camera);
            //DrawGridUI();
            DrawTextureTiled(bgTexture, (Rectangle){0.0f, 0.0f, 1920.0f, 1280.0f}, (Rectangle){-offMapSize * SQUARE_SIZE, -offMapSize * SQUARE_SIZE, 3000.0f, 2000.0f}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f, WHITE);
            DrawTextureTiled(fgTexture, (Rectangle){0.0f, 0.0f, 516.0f, 516.0f}, (Rectangle){0.0f, 0.0f, mapWidth * SQUARE_SIZE, mapHeight * SQUARE_SIZE}, (Vector2){0.0f, 0.0f}, 0.0f, 1.6f, WHITE);
            DrawRectangle(0, 0, SQUARE_SIZE, mapHeight * SQUARE_SIZE, DARKBROWN);
            DrawRectangle(SQUARE_SIZE, 0, mapWidth * SQUARE_SIZE - SQUARE_SIZE, SQUARE_SIZE, DARKBROWN);
            DrawRectangle(SQUARE_SIZE, mapHeight * SQUARE_SIZE - SQUARE_SIZE, mapWidth * SQUARE_SIZE - SQUARE_SIZE, SQUARE_SIZE, DARKBROWN);
            DrawRectangle(mapWidth * SQUARE_SIZE - SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, mapHeight * SQUARE_SIZE - SQUARE_SIZE * 2, DARKBROWN);

            // Draw snake
            for (int i = 0; i < counterTail; i++) DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);

            // Draw fruit to pick
            //DrawRectangleV(fruit.position, fruit.size, fruit.color);
            for (int i = 0; i < FOOD_ITEMS; i++)
            DrawCircleV((Vector2){fruits[i].position.x + fruits[i].size.x / 2, fruits[i].position.y + fruits[i].size.x / 2}, fruits[i].size.x / 2, fruits[i].color);
        
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
    UnloadTexture(bgTexture);
}

// Update and Draw (one frame)
void UpdateDrawFrame(Camera2D* camera)
{
    UpdateGame(camera);
    DrawGame(camera);
}

// Draws pause text in topright and middle when paused
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
    for (int i = 0; i < FOOD_ITEMS; i++)
    DrawText(TextFormat("Bonus time: %02i", fruits[i].lifetime), 30, 60 + i * 30, 24, MAROON);
}

void DrawGridUI(void)
{
    
    //DrawRectangle(0, 0, mapWidth * SQUARE_SIZE, mapHeight * SQUARE_SIZE, GREEN);
    // for (int i = 0; i < mapWidth + 1; i++)
    // {
    //     DrawLineV((Vector2){SQUARE_SIZE*i, 0}, (Vector2){SQUARE_SIZE*i, mapHeight * SQUARE_SIZE}, LIGHTGRAY);
    // }

    // for (int i = 0; i < mapHeight + 1; i++)
    // {
    //     DrawLineV((Vector2){0, SQUARE_SIZE*i}, (Vector2){mapWidth * SQUARE_SIZE, SQUARE_SIZE*i}, LIGHTGRAY);
    // }

    DrawRectangle(0, 0, SQUARE_SIZE, mapHeight * SQUARE_SIZE, DARKBROWN);
    DrawRectangle(SQUARE_SIZE, 0, mapWidth * SQUARE_SIZE - SQUARE_SIZE, SQUARE_SIZE, DARKBROWN);
    DrawRectangle(SQUARE_SIZE, mapHeight * SQUARE_SIZE - SQUARE_SIZE, mapWidth * SQUARE_SIZE - SQUARE_SIZE, SQUARE_SIZE, DARKBROWN);
    DrawRectangle(mapWidth * SQUARE_SIZE - SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, mapHeight * SQUARE_SIZE - SQUARE_SIZE * 2, DARKBROWN);
}

void UpdateCameraCenterInsideMap(Camera2D *camera, Snake *player)
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
