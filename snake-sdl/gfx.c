#include "gfx.h"

static uint16_t window_width;
static uint16_t window_height;
static uint16_t tile_size;

static SDL_Window *mainWindow;
static SDL_Renderer *renderer;

static SDL_Texture *texture_bg;
static SDL_Texture *texture_head;
static SDL_Texture *texture_tail;
static SDL_Texture *texture_apple;
static SDL_Texture *texture_border;

static void load_texture(char *path, SDL_Texture **texture_ptr)
{
    SDL_Surface *bmp = SDL_LoadBMP(path);
    *texture_ptr = SDL_CreateTextureFromSurface(renderer, bmp);
    SDL_FreeSurface(bmp);
}

static void render_texture(SDL_Texture *texture, int x, int y)
{
    SDL_Rect destination = {.x = x, .y = y};
    SDL_QueryTexture(texture, NULL, NULL, &destination.w, &destination.h);
    SDL_RenderCopy(renderer, texture, NULL, &destination);
}

void gfx_init(int new_window_width, int new_window_height, int new_tile_size)
{
    window_width = new_window_width;
    window_height = new_window_height;
    tile_size = new_tile_size;
    char * title = "Test Window";

    SDL_Init(SDL_INIT_VIDEO);
    mainWindow = SDL_CreateWindow(title, 0, 0, window_width*tile_size, window_height*tile_size, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    load_texture("bg.bmp", &texture_bg);
    load_texture("head.bmp", &texture_head);
    load_texture("tail.bmp", &texture_tail);
    load_texture("apple.bmp", &texture_apple);
    load_texture("border.bmp", &texture_border);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture_bg, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_RaiseWindow(mainWindow);
}

void gfx_exit(void)
{
    SDL_DestroyTexture(texture_bg);
    SDL_DestroyTexture(texture_head);
    SDL_DestroyTexture(texture_tail);
    SDL_DestroyTexture(texture_apple);
    SDL_DestroyTexture(texture_border);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();
}

void gfx_draw(GfxElement_t element, int x, int y)
{
    SDL_Texture *texture;

    switch (element) 
    {
        case GFX_NONE:
            texture = texture_bg;
            break;
        case GFX_HEAD:
            texture = texture_head;
            break;
        case GFX_TAIL:
            texture = texture_tail;
            break;
        case GFX_APPLE:
            texture = texture_apple;
            break;
        case GFX_BORDER:
            texture = texture_border;
            break;
        default:
            return;
    }

    render_texture(texture, x * tile_size, y * tile_size);
}

void gfx_clear(void)
{
    SDL_RenderClear(renderer);
}

void gfx_present(void)
{
    SDL_RenderPresent(renderer);
}
