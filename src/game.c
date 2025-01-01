#include "game.h"
#include "raylib.h"
#include "stdlib.h"

/***********************************************game settings***********************************************/

int quit = 0;
game_state state = TITLE;
const int screen_width = 800;
const int screen_height = 600;
const float gravity = 2.0f;
int frame_count = 0;

/***********************************************gameplay elements***********************************************/

//players
struct player
{
    Vector2 position;
    Vector2 size;
    float speed;
    Rectangle bounds;
} player, player2;

//ball
struct ball
{
    Vector2 position;
    float radius;
    Vector2 speed;
} ball;

/***********************************************assets***********************************************/

Font font;
Sound bounce_sound;
Sound explosion_sound;
Sound start_sound;

/***********************************************init***********************************************/

void game_init()
{
    //init configs
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(screen_width, screen_height, "Hello raylib: Pong");
    SetTargetFPS(60);
    InitAudioDevice();

    //load assets
    font = LoadFont("./resources/setback.png");
    bounce_sound = LoadSound("./resources/bounce.wav");
    start_sound = LoadSound("./resources/start.wav");

    //init player 1
    player.position = (Vector2){ screen_width / 4, screen_height / 2 };
    player.size = (Vector2){20.0f, 120.0f};
    player.speed = 8.0f;
    player.bounds.x = player.position.x;
    player.bounds.y = player.position.y;
    player.bounds.width = player.size.x;
    player.bounds.height = player.size.y;

    //init player 2
    player2.position = (Vector2){ screen_width / 4 * 3, screen_height / 2 };
    player2.size = (Vector2){20.0f, 120.0f};
    player2.speed = 8.0f;
    player2.bounds.x = player.position.x;
    player2.bounds.y = player.position.y;
    player2.bounds.width = player.size.x;
    player2.bounds.height = player.size.y;

    //int ball
    ball.radius = 5.0f;
    ball.position = (Vector2){ screen_width / 2, screen_height / 2 };
    ball.speed.x = 4.0f;
    ball.speed.y = 4.0f;
}

/***********************************************state = TITLE***********************************************/

void game_title_process_input(void)
{
    //quit if player press X window
    if(WindowShouldClose())
        quit = 1;

    //go to main game if player press enter
    if(IsKeyPressed(KEY_ENTER))
    {
        PlaySound(start_sound);
        state = PLAY;
    }
}

void game_title_update(void)
{
    frame_count++;
}

void game_title_draw(void)
{
    BeginDrawing();

    //clear
    ClearBackground(BLACK);
    //draw title
    Vector2 font_position = {160, 100};
    DrawTextEx(font, "Pong", font_position, 100.0f, 1.0f, RED);
    //draw blink press [enter] to play
    if(frame_count / 60.0f > 2)
    {
        DrawText("press [enter] to play", 100, 300, 50, DARKGREEN);
        if(frame_count / 60.0f > 2.5f)
            frame_count = 0;
    }

    EndDrawing();
}

/***********************************************state = PLAY***********************************************/

void game_play_process_input(void)
{
    //quit if player press X windows
    if(WindowShouldClose())
        quit = 1;

    //move player1 up
    if(IsKeyDown('W'))
        player.position.y -= player.speed;
    //move player1 down
    if(IsKeyDown('S'))
        player.position.y += player.speed;

    //move player2 up
    if(IsKeyDown(KEY_UP))
        player2.position.y -= player2.speed;
    //move player1 down
    if(IsKeyDown(KEY_DOWN))
        player2.position.y += player2.speed;
}

void game_play_update(void)
{

    //update player1 position and confine him on screen
    if(player.position.y <= 0)
        player.position.y = 0;
    if(player.position.y + player.size.y > screen_height)
        player.position.y = screen_height - player.size.y;

    //update player2 position and confine him on screen
    if(player2.position.y <= 0)
        player2.position.y = 0;
    if(player2.position.y + player2.size.y > screen_height)
        player2.position.y = screen_height - player2.size.y;

    //update player1 collisions
    player.bounds.x = player.position.x;
    player.bounds.y = player.position.y;

    //update player2 collisions
    player2.bounds.x = player2.position.x;
    player2.bounds.y = player2.position.y;

    //update ball position
    ball.position.x += ball.speed.x;
    ball.position.y += ball.speed.y;

    //ball collision right & left
    if(ball.position.x - ball.radius <= 0 || ball.position.x + ball.radius >= screen_width)
    {
        ball.speed.x *= -1;
        //sound
        PlaySound(bounce_sound);
    }

    //ball collision up & down
    if(ball.position.y - ball.radius <= 0 || ball.position.y + ball.radius >= screen_height)
    {
        ball.speed.y *= -1;
        //sound
        PlaySound(bounce_sound);
    }
    
    //check collision ball <-> player1
    if(CheckCollisionCircleRec(ball.position, ball.radius, player.bounds))
    {
        //bounce up based on collision point
        ball.speed.y = (ball.position.y - (player.position.y + player.size.y / 2)) / player.size.y * 5.0f;
        ball.speed.x *= -1;
        //sound
        PlaySound(bounce_sound);
    }

    //check collision ball <-> player2
    if(CheckCollisionCircleRec(ball.position, ball.radius, player2.bounds))
    {
        //bounce up based on collision point
        ball.speed.y = (ball.position.y - (player2.position.y + player2.size.y / 2)) / player2.size.y * 5.0f;
        ball.speed.x *= -1;
        //sound
        PlaySound(bounce_sound);
    }

}

void game_play_draw(void)
{
    BeginDrawing();

    //clear
    ClearBackground(BLUE);
    //draw players
    DrawRectangle(player.position.x, player.position.y, player.size.x, player.size.y, WHITE);
    DrawRectangle(player2.position.x, player2.position.y, player2.size.x, player2.size.y, WHITE);
    //draw ball
    DrawCircleV(ball.position, ball.radius, WHITE);

    EndDrawing();
}

/***********************************************state = GAME OVER***********************************************/

void game_over_process_input(void)
{
    //quit if player press windows X
    if(WindowShouldClose())
        quit = 1;

    //reset and re-enter in game if player press enter
    if(IsKeyPressed(KEY_ENTER))
    {
        state = TITLE;
    }
}

void game_over_update(void)
{
    frame_count++;
}

void game_over_draw(void)
{
    BeginDrawing();

    //clear
    ClearBackground(BLACK);
    //draw title
    Vector2 font_position = {160, 100};
    DrawTextEx(font, "Game Over", font_position, 100.0f, 1.0f, RED);
    //draw blink press [enter] to play
    if(frame_count / 60.0f > 2)
    {
        DrawText("press [enter] to play", 100, 300, 50, DARKGREEN);
        if(frame_count / 60.0f > 2.5f)
            frame_count = 0;
    }

    EndDrawing();
}

/***********************************************deinit***********************************************/

void game_deinit(void)
{
    UnloadSound(bounce_sound);
    UnloadSound(explosion_sound);
    UnloadSound(start_sound);
}