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
#include "gfx.h"

/*
 * game defs
 */

#define WIDTH (32)
#define HEIGHT (16)
#define LOGLINE_X (WIDTH/4)
#define LOGLINE_Y (HEIGHT+3)
#define APPLE_MAX_COUNT (4)
#define TAIL_MAX_LENGTH (128)
#define INPUT_TICK_MS (32)
#define GAME_TICK_MS (64)
#define APPLE_RESPAWN_MS (1000)

typedef struct GameState
{
    bool running;
    uint8_t ms_since_game_tick;
    uint8_t ms_since_input_tick;
    uint16_t ms_since_apple_spawn;
    uint8_t head_x;
    uint8_t head_y;
    int8_t dir_x;
    int8_t dir_y;
    uint8_t next_apple_x;
    uint8_t next_apple_y;
    int8_t next_apple_idx;
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
    .ms_since_game_tick = 0,
    .ms_since_input_tick = 0,
    .ms_since_apple_spawn = 0,
    .running = false,
    .head_x = WIDTH / 2,
    .head_y = HEIGHT / 2,
    .dir_x = 0,
    .dir_y = 0,
    .next_apple_x = 0,
    .next_apple_y = 0,
    .next_apple_idx = 0,
    .tail_length = 0,
    .apple_coords = {{-1}},
    .tail_coords = {{-1}}
};

void gotoxy(uint8_t x, uint8_t y)
{
    printf("\033[%d;%dH", (y), (x));
}

void print(char *text)
{
    gotoxy(LOGLINE_X, LOGLINE_Y);
    printf("%s", text);
    fflush(stdin);
}

void clear_frame(void)
{
    printf("\033[H\033[J");
    gfx_clear();
}

void draw_frame_static(void)
{
    uint16_t idx = 0;

    for (uint16_t x = 0; x < WIDTH+4; x++)
    {
        for (uint16_t y = 0; y < HEIGHT+4; y++)
        {
            gfx_draw(GFX_NONE, x, y);
        }
    }

    for (idx = 1; idx <= WIDTH+2; idx++)
    {
        gotoxy(idx, 1);
        printf("%c", chars[4]);
        gotoxy(idx, HEIGHT+2);
        printf("%c", chars[4]);

        gfx_draw(GFX_BORDER, idx, 1);
        gfx_draw(GFX_BORDER, idx, HEIGHT+2);
    }

    for (idx = 1; idx <= HEIGHT+2; idx++)
    {
        gotoxy(1, idx);
        printf("%c", chars[4]);
        gotoxy(WIDTH+2, idx);
        printf("%c", chars[4]);

        gfx_draw(GFX_BORDER, 1, idx);
        gfx_draw(GFX_BORDER, WIDTH+2, idx);
    }
}

void draw_frame_dynamic(bool wipe)
{
    uint16_t idx = 0;

    for (idx = 0; idx < TAIL_MAX_LENGTH; idx++)
    {
        if (game.tail_coords[idx][0] < 0) break;
        gotoxy(game.tail_coords[idx][0], game.tail_coords[idx][1]);
        printf("%c", chars[wipe ? 0 : 2]);

        if (wipe)
        {
            gfx_draw(GFX_NONE, game.tail_coords[idx][0], game.tail_coords[idx][1]);
        }
        else if (game.dir_x == 0)
        {
            gfx_draw_scaled(GFX_TAIL, game.tail_coords[idx][0], game.tail_coords[idx][1], 1.0 / (1+(idx/20.0)), 1.0);
        }
        else
        {
            gfx_draw_scaled(GFX_TAIL, game.tail_coords[idx][0], game.tail_coords[idx][1], 1.0, 1.0 / (1+(idx/20.0)));
        }
    }

    for (idx = 0; idx < APPLE_MAX_COUNT; idx++)
    {
        if (game.apple_coords[idx][0] < 0) continue;
        gotoxy(game.apple_coords[idx][0], game.apple_coords[idx][1]);
        printf("%c", chars[wipe ? 0 : 3]);

        gfx_draw(wipe ? GFX_NONE : GFX_APPLE, game.apple_coords[idx][0], game.apple_coords[idx][1]);
    }

    gotoxy(game.head_x, game.head_y);
    printf("%c", chars[wipe ? 0 : 1]);
    gfx_draw(wipe ? GFX_NONE : GFX_HEAD, game.head_x, game.head_y);
}

void game_over(uint16_t collided_idx)
{
    uint16_t gap = 16;
    int16_t idx = 0;

    delay_ms(gap);

    for (idx = game.tail_length - 1; idx >= 0; idx--)
    {
        if (idx == collided_idx) continue;
        gotoxy(game.tail_coords[idx][0], game.tail_coords[idx][1]);
        printf("%c", ',');
        fflush(stdout);
        gfx_draw(GFX_TAIL, game.tail_coords[idx][0], game.tail_coords[idx][1]);
        gfx_present();
        delay_ms(gap);
    }

    delay_ms(gap);

    for (idx = game.tail_length - 1; idx >= 0; idx--)
    {
        if (idx == collided_idx) continue;
        gotoxy(game.tail_coords[idx][0], game.tail_coords[idx][1]);
        printf("%c", '.');
        fflush(stdout);
        gfx_draw(GFX_APPLE, game.tail_coords[idx][0], game.tail_coords[idx][1]);
        gfx_present();
        delay_ms(gap);
    }

    delay_ms(gap);

    for (idx = game.tail_length - 1; idx >= 0; idx--)
    {
        if (idx == collided_idx) continue;
        gotoxy(game.tail_coords[idx][0], game.tail_coords[idx][1]);
        printf("%c", ' ');
        fflush(stdout);
        gfx_draw(GFX_NONE, game.tail_coords[idx][0], game.tail_coords[idx][1]);
        gfx_present();
        delay_ms(gap);
    }

    delay_ms(gap);
    delay_ms(gap);

    gotoxy(game.head_x, game.head_y);
    printf("%c", 'x');
    fflush(stdout);
    gfx_draw(GFX_TAIL, game.head_x, game.head_y);
    gfx_present();
    delay_ms(gap);
    delay_ms(gap);

    gotoxy(game.head_x, game.head_y);
    printf("%c", ',');
    fflush(stdout);
    gfx_draw(GFX_APPLE, game.head_x, game.head_y);
    gfx_present();
    delay_ms(gap);
    delay_ms(gap);

    gotoxy(game.head_x, game.head_y);
    printf("%c", '.');
    fflush(stdout);
    gfx_draw(GFX_NONE, game.head_x, game.head_y);
    gfx_present();
    delay_ms(gap);
    delay_ms(gap);

    print("-------- Game Over --------\n");
    poll(&pollReq, 1, -1);
}

bool handle_input(void)
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
        case 'x':
            return true;
        default:
            return false;
    }

    game.running = true;
    return false;
}

void determine_next_apple_pos(void)
{
    bool success = false;
    uint8_t next_x;
    uint8_t next_y;
    uint16_t idx;

    while (!success)
    {
        next_x = (rand() % (WIDTH)) + 2;
        next_y = (rand() % (HEIGHT)) + 2;

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

    //char next_apple_string[] = "Apple Next X: xx, Y: xx";
    //sprintf(next_apple_string, "Apple Next X: %2u, Y: %2u", next_x, next_y);
    //print(next_apple_string);
}

void handle_apple_spawning(void)
{
    if (game.next_apple_idx >= 0)
    {
        if (game.ms_since_apple_spawn >= APPLE_RESPAWN_MS)
        {
            uint8_t idx = 0;

            game.apple_coords[game.next_apple_idx][0] = game.next_apple_x;
            game.apple_coords[game.next_apple_idx][1] = game.next_apple_y;
            game.ms_since_apple_spawn = 0;

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
            game.ms_since_apple_spawn += GAME_TICK_MS;
        }
    }
}

void handle_apple_collision(uint8_t apple_idx)
{
    game.tail_length++;

    game.apple_coords[apple_idx][0] = -1;
    game.apple_coords[apple_idx][1] = -1;
    game.next_apple_idx = apple_idx;
    game.ms_since_apple_spawn = 0;

    char tail_length_string[] = "Tail Length:   0   \n";
    sprintf(tail_length_string, "Tail Length: %3u   \n", game.tail_length);
    print(tail_length_string);
}

bool detect_collision(void)
{
    uint16_t idx = 0;

    for (idx = 0; idx < TAIL_MAX_LENGTH; idx++)
    {
        if (game.tail_coords[idx][0] < 0) break;

        if (game.tail_coords[idx][0] == game.head_x
                && game.tail_coords[idx][1] == game.head_y)
        {
            game_over(idx);
            return true;
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
    
    return false;
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

    if (game.head_x < 2) game.head_x = WIDTH+1;
    else if (game.head_x > WIDTH+1) game.head_x = 2;

    if (game.head_y < 2) game.head_y = HEIGHT+1;
    else if (game.head_y > HEIGHT+1) game.head_y = 2;
}

bool inner_loop(void)
{
    //char status[] = "GAME: XX, INPUT: XX";

    while(true)
    {
        delay_ms(1);
        //sprintf(status, "Game: %2u, Input: %2u", game.ms_since_game_tick, game.ms_since_input_tick);
        //print(status);

        if (game.ms_since_input_tick >= INPUT_TICK_MS)
        {
            game.ms_since_input_tick = 0;

            if(poll(&pollReq, 1, 0))
            {
                if(handle_input())
                {
                    print("Thank you for playing! --------\n");
                    return true;
                }
            }
        }
        else game.ms_since_input_tick += 1;

        if (!game.running) continue;

        if (game.ms_since_game_tick >= GAME_TICK_MS)
        {
            game.ms_since_game_tick = 0;

            if (detect_collision()) break;

            draw_frame_dynamic(true);
            handle_movement();
            handle_apple_spawning();
            draw_frame_dynamic(false);
            fflush(stdout);
            gfx_present();
        }
        else game.ms_since_game_tick += 1;
    }

    return false;
}

void inner_init()
{
    uint16_t idx = 0;

    game.running = false;
    game.ms_since_game_tick = 0;
    game.ms_since_input_tick = 0;
    game.head_x = WIDTH / 2;
    game.head_y = HEIGHT / 2;
    game.dir_x = 0;
    game.dir_y = 0;
    game.ms_since_apple_spawn = 0;
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

    determine_next_apple_pos();
    clear_frame();

    draw_frame_static();

    fflush(stdin);
    gfx_present();
    delay_ms(50);

    draw_frame_dynamic(false);
    print("h, j, k, l to move, x to quit\n");

    fflush(stdin);
    gfx_present();
    delay_ms(50);
}

void outer_loop(void)
{
    while(true)
    {
        inner_init();
        if(inner_loop()) break;
    }
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

    gfx_init(WIDTH+4, HEIGHT+4, 32);
}

int main(void)
{
    outer_init();
    outer_loop();
    gfx_exit();
}
