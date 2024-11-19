#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "delay.h"

#define WIDTH 80
#define HEIGHT 24

/*
 * system vars
 */

struct pollfd pollReq =
{
    .fd = STDIN_FILENO,
    .events = POLLIN
};

struct termios config;

/*
 * game vars
 */

typedef struct GameState
{
    bool running;
    uint8_t head_x;
    uint8_t head_y;
    int8_t dir_x;
    int8_t dir_y;
    int8_t apple_coords[4][2]; 
    int8_t tail_coords[256][2];
} GameState_t;

GameState_t gameState =
{
    .running = false,
    .head_x = WIDTH / 2,
    .head_y = HEIGHT / 2,
    .dir_x = 0,
    .dir_y = 0,
    .apple_coords = {-1},
    .tail_coords = {-1}
};

char chars[5] = {' ', 'O', 'o', '@', 'X'};

void gotoxy(uint8_t x, uint8_t y)
{
    printf("\033[%d;%dH", (y), (x));
}

void clear_frame(void)
{
    printf("\033[H\033[J");
}

void draw_frame_static(void)
{
    uint16_t idx = 0;

    for (idx = 0; idx < WIDTH; idx++)
    {
        gotoxy(idx, 0);
        printf("%c", chars[4]);
        gotoxy(idx, HEIGHT-1);
        printf("%c", chars[4]);
    }

    for (idx = 0; idx < HEIGHT; idx++)
    {
        gotoxy(0, idx);
        printf("%c", chars[4]);
        gotoxy(WIDTH-1, idx);
        printf("%c", chars[4]);
    }
}

void draw_frame_dynamic(void)
{
    uint16_t idx = 0;
    
    gotoxy(gameState.head_x, gameState.head_y);
    printf("%c", chars[1]);

    for (idx = 0; idx < 256; idx++)
    {
        if (gameState.tail_coords[idx][0] < 0) break;
        gotoxy(gameState.tail_coords[idx][0], gameState.tail_coords[idx][1]);
        printf("%c", chars[2]);
    }

    for (idx = 0; idx < 4; idx++)
    {
        if (gameState.apple_coords[idx][0] < 0) break;
        gotoxy(gameState.apple_coords[idx][0], gameState.apple_coords[idx][1]);
        printf("%c", chars[3]);
    }

    fflush(stdout);
}

void handle_input(void)
{
    char input = getchar();

    switch (input)
    {
        case 'h':
            gameState.dir_x = -1;
            gameState.dir_y = 0;
            break;
        case 'j':
            gameState.dir_x = 0;
            gameState.dir_y = 1;
            break;
        case 'k':
            gameState.dir_x = 0;
            gameState.dir_y = -1;
            break;
        case 'l':
            gameState.dir_x = 1;
            gameState.dir_y = 0;
            break;
        default:
            return;
    }

    gameState.running = true;
}

void handle_movement(void)
{
    gameState.head_x += gameState.dir_x;
    gameState.head_y += gameState.dir_y;

    if (gameState.head_x <= 1) gameState.head_x = WIDTH - 2;
    else if (gameState.head_x >= WIDTH-1) gameState.head_x = 1;

    if (gameState.head_y <= 1) gameState.head_y = HEIGHT - 2;
    else if (gameState.head_y >= HEIGHT-1) gameState.head_y = 1;
}

void inner_loop(void)
{
    while(true)
    {
        if(poll(&pollReq, 1, 24))
        {
            handle_input();
        }

        if (!gameState.running) continue;

        handle_movement();
        clear_frame();
        draw_frame_static();
        draw_frame_dynamic();
        delay_ms(10);
    }
}

void inner_init()
{
    gameState.running = false;
    gameState.head_x = WIDTH / 2;
    gameState.head_y = HEIGHT / 2;
    gameState.dir_x = 0;
    gameState.dir_y = 0;
    draw_frame_dynamic();
}

void outer_loop(void)
{
    inner_init();
    inner_loop();
}

void outer_init(void)
{
    srand(time(NULL));

    // configure termios
    if (tcgetattr(STDIN_FILENO, &config) < 0)
    {
        // TODO: error handling
    }

    config.c_cflag |= (CLOCAL | CREAD);
    config.c_lflag &= ~(ICANON | ECHO | ECHOE);
    config.c_oflag &= ~OPOST;
    config.c_cc[VMIN] = 1;
    config.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &config);
}

int main(void)
{
    outer_init();
    outer_loop();
}
