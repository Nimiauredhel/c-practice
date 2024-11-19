#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "delay.h"

/*
 * game defs
 */

#define WIDTH 48
#define HEIGHT 24
#define APPLE_MAX_COUNT 4
#define TAIL_MAX_LENGTH 256
#define GAME_TICK_MS 64
#define APPLE_RESPAWN_MS 1600

typedef struct GameState
{
    bool running;
    uint8_t head_x;
    uint8_t head_y;
    int8_t dir_x;
    int8_t dir_y;
    uint8_t next_apple_x;
    uint8_t next_apple_y;
    uint8_t next_apple_idx;
    uint16_t apple_timer;
    uint16_t tail_length;
    int8_t apple_coords[APPLE_MAX_COUNT][2]; 
    int8_t tail_coords[TAIL_MAX_LENGTH][2];
} GameState_t;

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

const char chars[5] = {' ', 'O', 'o', '@', 'X'};

GameState_t game =
{
    .running = false,
    .head_x = WIDTH / 2,
    .head_y = HEIGHT / 2,
    .dir_x = 0,
    .dir_y = 0,
    .next_apple_x = 0,
    .next_apple_y = 0,
    .next_apple_idx = 0,
    .apple_timer = 0,
    .tail_length = 0,
    .apple_coords = {-1},
    .tail_coords = {-1}
};

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
    
    gotoxy(game.head_x, game.head_y);
    printf("%c", chars[1]);

    for (idx = 0; idx < TAIL_MAX_LENGTH; idx++)
    {
        if (game.tail_coords[idx][0] < 0) break;
        gotoxy(game.tail_coords[idx][0], game.tail_coords[idx][1]);
        printf("%c", chars[2]);
    }

    for (idx = 0; idx < APPLE_MAX_COUNT; idx++)
    {
        if (game.apple_coords[idx][0] < 0) continue;
        gotoxy(game.apple_coords[idx][0], game.apple_coords[idx][1]);
        printf("%c", chars[3]);
    }
}

void handle_input(void)
{
    char input = getchar();

    switch (input)
    {
        case 'h':
            game.dir_x = -1;
            game.dir_y = 0;
            break;
        case 'j':
            game.dir_x = 0;
            game.dir_y = 1;
            break;
        case 'k':
            game.dir_x = 0;
            game.dir_y = -1;
            break;
        case 'l':
            game.dir_x = 1;
            game.dir_y = 0;
            break;
        default:
            return;
    }

    game.running = true;
}

void determine_next_apple_pos(void)
{
    bool success = false;
    uint8_t next_x;
    uint8_t next_y;
    uint16_t idx;

    while (!success)
    {
        next_x = rand() % (WIDTH-3) + 3;
        next_y = rand() % (HEIGHT-3) + 3;

        success = true;

        if (game.head_x == next_x && game.head_y == next_y)
        {
            success = false;
            continue;
        }

        for (idx = 0; idx < TAIL_MAX_LENGTH; idx++)
        {
            if (game.tail_coords[idx][0] < 0) break;

            if (game.tail_coords[idx][0] == next_x
                    && game.tail_coords[idx][1] == next_y)
            {
                success = false;
                continue;
            }
        }

        for (idx = 0; idx < APPLE_MAX_COUNT; idx++)
        {
            if (game.apple_coords[idx][0] < 0) continue;

            if (game.apple_coords[idx][0] == next_x
                    && game.apple_coords[idx][1] == next_y)
            {
                success = false;
                continue;
            }
        }
    }

    game.next_apple_x = next_x;
    game.next_apple_y = next_y;
}

void handle_apple_spawning(void)
{
    if (game.next_apple_idx >= 0)
    {
        if (game.apple_timer >= APPLE_RESPAWN_MS)
        {
            uint8_t idx = 0;

            game.apple_coords[game.next_apple_idx][0] = game.next_apple_x;
            game.apple_coords[game.next_apple_idx][1] = game.next_apple_y;
            game.apple_timer = 0;

            game.next_apple_idx = -1;
            determine_next_apple_pos();

            for (idx = 0; idx < APPLE_MAX_COUNT; idx++)
            {
                if (game.apple_coords[idx][0] < 0)
                {
                    game.next_apple_idx = idx;
                    break;
                }
            }
        }
        else 
        {
            game.apple_timer += GAME_TICK_MS;
        }
    }
}

void handle_tail_collision(void)
{
    game.running = false;
}

void handle_apple_collision(uint8_t apple_idx)
{
    game.tail_length++;

    game.apple_coords[apple_idx][0] = -1;
    game.apple_coords[apple_idx][1] = -1;
    game.next_apple_idx = apple_idx;
    game.apple_timer = 0;
}

void detect_collision(void)
{
    uint16_t idx = 0;

    for (idx = 0; idx < TAIL_MAX_LENGTH; idx++)
    {
        if (game.tail_coords[idx][0] < 0) break;

        if (game.tail_coords[idx][0] == game.head_x
                && game.tail_coords[idx][1] == game.head_y)
        {
            handle_tail_collision();
        }
    }

    for (idx = 0; idx < APPLE_MAX_COUNT; idx++)
    {
        if (game.apple_coords[idx][0] < 0) continue;

        if (game.apple_coords[idx][0] == game.head_x
                && game.apple_coords[idx][1] == game.head_y)
        {
            handle_apple_collision(idx);
        }
    }

    if (game.head_x == game.next_apple_x
        && game.head_y == game.next_apple_y)
    {
        determine_next_apple_pos();
    }
}

void handle_movement(void)
{
    if (game.dir_x == 0 && game.dir_y == 0)
    {
        return;
    }

    if (game.tail_length > 0)
    {
        uint16_t idx = 0;

        for (idx = game.tail_length - 1; idx > 0; idx--)
        {
            game.tail_coords[idx][0] = game.tail_coords[idx-1][0];
            game.tail_coords[idx][1] = game.tail_coords[idx-1][1];
        }

        game.tail_coords[0][0] = game.head_x;
        game.tail_coords[0][1] = game.head_y;
    }

    game.head_x += game.dir_x;
    game.head_y += game.dir_y;

    if (game.head_x <= 1) game.head_x = WIDTH - 2;
    else if (game.head_x >= WIDTH-1) game.head_x = 2;

    if (game.head_y <= 1) game.head_y = HEIGHT - 2;
    else if (game.head_y >= HEIGHT-1) game.head_y = 2;
}

void inner_loop(void)
{
    while(true)
    {
        if(poll(&pollReq, 1, 24))
        {
            handle_input();
        }

        if (!game.running) continue;

        handle_movement();
        detect_collision();
        handle_apple_spawning();

        clear_frame();
        draw_frame_static();
        draw_frame_dynamic();
        fflush(stdout);

        delay_ms(GAME_TICK_MS);
    }
}

void inner_init()
{
    uint16_t idx = 0;

    game.running = false;
    game.head_x = WIDTH / 2;
    game.head_y = HEIGHT / 2;
    game.dir_x = 0;
    game.dir_y = 0;
    game.apple_timer = 0;
    game.tail_length = 0;
    game.next_apple_idx = 0;

    for (idx = 0; idx < TAIL_MAX_LENGTH; idx++)
    {
        memset(game.tail_coords[idx], -1, 2);
    }

    for (idx = 0; idx < APPLE_MAX_COUNT; idx++)
    {
        memset(game.apple_coords[idx], -1, 2);
    }

    clear_frame();
    draw_frame_static();
    draw_frame_dynamic();
    fflush(stdin);
    determine_next_apple_pos();
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
