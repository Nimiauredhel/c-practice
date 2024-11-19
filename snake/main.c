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

bool running = false;
int8_t head_x = WIDTH / 2;
int8_t head_y = HEIGHT / 2;
int8_t dir_x = 0;
int8_t dir_y = 0;
uint8_t grid[WIDTH][HEIGHT] = {0};

struct pollfd pollReq =
{
    .fd = STDIN_FILENO,
    .events = POLLIN
};

struct termios config;

char chars[5] = {' ', 'O', 'o', '@', 'X'};

void gotoxy(uint8_t x, uint8_t y)
{
    printf("\033[%d;%dH", (y), (x));
}

void clear(void)
{
    printf("\033[H\033[J");
}

void draw_frame(void)
{
    uint8_t current = 0;
    clear();

    for (uint8_t x = 0; x < WIDTH; x++)
    {
        for (uint8_t y = 0; y < HEIGHT; y++)
        {
            current = grid[x][y];

            if (current > 0)
            {
                gotoxy(x, y);
                printf("%c", chars[current]);
                grid[x][y] = current - 1;
            }
        }
    }

    fflush(stdout);
}

void clear_grid(void)
{
    for (uint8_t x = 0; x < WIDTH; x++)
    {
        for (uint8_t y = 0; y < HEIGHT; y++)
        {
            grid[x][y] = 0;
        }
    }
}

void handle_input(void)
{
    char input = getchar();

    switch (input)
    {
        case 'h':
            dir_x = -1;
            dir_y = 0;
            break;
        case 'j':
            dir_x = 0;
            dir_y = 1;
            break;
        case 'k':
            dir_x = 0;
            dir_y = -1;
            break;
        case 'l':
            dir_x = 1;
            dir_y = 0;
            break;
        default:
            return;
    }

    running = true;
}

void handle_movement(void)
{
    //grid[head_x][head_y] = 0;

    head_x += dir_x;
    head_y += dir_y;

    if (head_x < 0) head_x = WIDTH - 1;
    else if (head_x >= WIDTH) head_x = 0;

    if (head_y < 0) head_y = HEIGHT - 1;
    else if (head_y >= HEIGHT) head_y = 0;

    grid[head_x][head_y] = 4;
}

void inner_loop(void)
{
    while(true)
    {
        if(poll(&pollReq, 1, 24))
        {
            handle_input();
        }

        if (!running) continue;

        handle_movement();
        draw_frame();
        delay_ms(10);
    }
}

void inner_init()
{
    running = false;
    head_x = WIDTH / 2;
    head_y = HEIGHT / 2;
    dir_x = 0;
    dir_y = 0;
    clear_grid();
    grid[head_x][head_y] = 4;
    draw_frame();
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
