#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "delay.h"
#include "gfx.h"
#include "input.h"

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
    char current_message[64];
    int8_t apple_coords[APPLE_MAX_COUNT][2]; 
    int8_t tail_coords[TAIL_MAX_LENGTH][2];
    int8_t tail_dirs[TAIL_MAX_LENGTH][2];
} GameState_t;

/*
 * game vars
 */

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
    .tail_coords = {{-1}},
    .tail_dirs = {{-1}}
};

void clear_frame(void)
{
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
        gfx_draw(GFX_BORDER, idx, 1);
        gfx_draw(GFX_BORDER, idx, HEIGHT+2);
    }

    for (idx = 1; idx <= HEIGHT+2; idx++)
    {
        gfx_draw(GFX_BORDER, 1, idx);
        gfx_draw(GFX_BORDER, WIDTH+2, idx);
    }
}

void draw_frame_dynamic(bool wipe)
{
    uint16_t idx = 0;
    float tail_scale = 0.95;
    GfxElement_t tail_type = GFX_TAIL_STRAIGHT;
    double angle = 0.0;
    int8_t x_dir;
    int8_t y_dir;
    int8_t next_x_dir;
    int8_t next_y_dir;

    for (idx = 0; idx < TAIL_MAX_LENGTH; idx++)
    {
        if (game.tail_coords[idx][0] < 0) break;

        if (wipe)
        {
            gfx_draw(GFX_NONE, game.tail_coords[idx][0], game.tail_coords[idx][1]);
        }
        else
        {
            x_dir = game.tail_dirs[idx][0];
            y_dir = game.tail_dirs[idx][1];

            if (idx < TAIL_MAX_LENGTH - 1)
            {
                next_x_dir = game.tail_dirs[idx+1][0];
                next_y_dir = game.tail_dirs[idx+1][1];
            }
            else
            {
                next_x_dir = x_dir;
                next_y_dir = y_dir;
            }

            tail_scale *= 0.99;

            if (x_dir == next_x_dir
                || y_dir == next_y_dir)
            {
                tail_type = GFX_TAIL_STRAIGHT;

                if (x_dir == 0)
                {
                    angle = 90.0;
                }
                else
                {
                    angle = 0.0;
                }
            }
            else
            {
                tail_type = GFX_TAIL_CORNER;

                if (x_dir == 1)
                {
                    if (next_y_dir == 1)
                    {
                        // right down
                        angle = -90.0;
                    }
                    else
                    {
                        // right up
                        angle = 0.0;
                    }
                }
                else if (x_dir == -1)
                {
                    if (next_y_dir == 1)
                    {
                        // left down
                        angle = 180.0;
                    }
                    else
                    {
                        // left up
                        angle = 90.0;
                    }
                }
                else if (y_dir == 1)
                {
                    if (next_x_dir == 1)
                    {
                        // down right
                        angle = 90.0;
                    }
                    else
                    {
                        // down left
                        angle = 0.0;
                    }
                }
                else if (y_dir == -1)
                {
                    if (next_x_dir == 1)
                    {
                        // up right
                        angle = 180.0;
                    }
                    else
                    {
                        // up left
                        angle = 270.0;
                    }
                }
            }

            gfx_draw_ex(tail_type, game.tail_coords[idx][0], game.tail_coords[idx][1], 1.0, tail_scale, angle);
        }
    }

    for (idx = 0; idx < APPLE_MAX_COUNT; idx++)
    {
        if (game.apple_coords[idx][0] < 0) continue;

        gfx_draw(wipe ? GFX_NONE : GFX_APPLE, game.apple_coords[idx][0], game.apple_coords[idx][1]);
    }

    gfx_draw_ex(wipe ? GFX_NONE : GFX_HEAD, game.head_x, game.head_y, 1.0, 1.0,
            game.dir_x == 1 ? 180.0 : game.dir_x == -1 ? 0.0 : game.dir_y == 1 ? -90.0 : 90.0);
}

void game_over(uint16_t collided_idx)
{
    uint16_t gap = 16;
    int16_t idx = 0;

    delay_ms(gap);

    for (idx = game.tail_length - 1; idx >= 0; idx--)
    {
        if (idx == collided_idx) continue;
        gfx_draw(GFX_TAIL_STRAIGHT, game.tail_coords[idx][0], game.tail_coords[idx][1]);
        gfx_present();
        delay_ms(gap);
    }

    delay_ms(gap);

    for (idx = game.tail_length - 1; idx >= 0; idx--)
    {
        if (idx == collided_idx) continue;
        gfx_draw(GFX_APPLE, game.tail_coords[idx][0], game.tail_coords[idx][1]);
        gfx_present();
        delay_ms(gap);
    }

    delay_ms(gap);

    for (idx = game.tail_length - 1; idx >= 0; idx--)
    {
        if (idx == collided_idx) continue;
        gfx_draw(GFX_NONE, game.tail_coords[idx][0], game.tail_coords[idx][1]);
        gfx_present();
        delay_ms(gap);
    }

    delay_ms(gap*2);

    gfx_draw(GFX_TAIL_STRAIGHT, game.head_x, game.head_y);
    gfx_present();
    delay_ms(gap*2);

    gfx_draw(GFX_APPLE, game.head_x, game.head_y);
    gfx_present();
    delay_ms(gap*2);

    gfx_draw(GFX_NONE, game.head_x, game.head_y);
    gfx_present();
    delay_ms(gap*2);

    sprintf(game.current_message, "-- Game Over --");
    printf("-------- Game Over --------\n");
    SDL_PollEvent(NULL);
}

bool handle_input(SDL_Event *e)
{
    switch (e->type)
    {
        case SDL_QUIT:
        case SDL_APP_TERMINATING:
            return true;
        case SDL_KEYDOWN:
            break;
        case SDL_KEYUP:
        default:
            return false;
    }

    switch (e->key.keysym.sym)
    {
        case SDLK_h:
        case SDLK_LEFT:
        case SDLK_a:
            if (game.dir_x != 0) return false;
            game.dir_x = -1;
            game.dir_y = 0;
            break;
        case SDLK_j:
        case SDLK_DOWN:
        case SDLK_s:
            if (game.dir_y != 0) return false;
            game.dir_x = 0;
            game.dir_y = 1;
            break;
        case SDLK_k:
        case SDLK_UP:
        case SDLK_w:
            if (game.dir_y != 0) return false;
            game.dir_x = 0;
            game.dir_y = -1;
            break;
        case SDLK_l:
        case SDLK_RIGHT:
        case SDLK_d:
            if (game.dir_x != 0) return false;
            game.dir_x = 1;
            game.dir_y = 0;
            break;
        case SDLK_x:
        case SDLK_ESCAPE:
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

    sprintf(game.current_message, "Tail Length: %3u", game.tail_length);
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
            game.tail_dirs[idx][0] = game.tail_dirs[idx-1][0];
            game.tail_dirs[idx][1] = game.tail_dirs[idx-1][1];
        }

        game.tail_coords[0][0] = game.head_x;
        game.tail_coords[0][1] = game.head_y;
        game.tail_dirs[0][0] = game.dir_x;
        game.tail_dirs[0][1] = game.dir_y;
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
    static const uint8_t clear_after_x_draws = 4;
    uint8_t draw_counter = 0;

    while(true)
    {
        delay_ms(1);

        if (game.ms_since_input_tick >= INPUT_TICK_MS)
        {
            game.ms_since_input_tick = 0;

            SDL_Event e;

            if(SDL_PollEvent(&e))
            {
                if(handle_input(&e))
                {
                    sprintf(game.current_message, "Thank you for playing!");
                    printf("Thank you for playing! --------\n");
                    gfx_clear();
                    render_text(game.current_message);
                    gfx_present();
                    delay_ms(1000);
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

            if (draw_counter >= clear_after_x_draws)
            {
                draw_counter = 0;
                gfx_clear();
                draw_frame_static();
                render_text(game.current_message);
            }
            else
            {
                draw_counter++;
            }

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

    memset(game.current_message, '\0', 64);

    for (idx = 0; idx < TAIL_MAX_LENGTH; idx++)
    {
        memset(game.tail_coords[idx], -1, 2);
        memset(game.tail_dirs[idx], 0, 2);
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
    sprintf(game.current_message, "h, j, k, l to move, x to quit.");
    printf("h, j, k, l to move, x to quit\n");
    render_text(game.current_message);

    fflush(stdin);
    gfx_present();
    delay_ms(150);
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

    gfx_init(WIDTH+4, HEIGHT+4, 32);
    input_init();
}

int main(void)
{
    outer_init();
    outer_loop();
    gfx_exit();

    SDL_Quit();
}
