#include "gfx.h"

static const SDL_Color WHITE = { 255, 255, 255, 255 };

static uint16_t window_width;
static uint16_t window_height;
static uint16_t tile_size;

static SDL_Window *mainWindow;
static SDL_Renderer *renderer;

static SDL_Texture *texture_bg;
static SDL_Texture *texture_head;
static SDL_Texture *texture_tail_straight;
static SDL_Texture *texture_tail_corner;
static SDL_Texture *texture_tail_end;
static SDL_Texture *texture_apple;
static SDL_Texture *texture_border;

static SDL_Texture *text_texture;
static TTF_Font *font_sans;

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

static void render_texture_ex(SDL_Texture *texture, int x, int y, float x_scale, float y_scale, double angle)
{
    SDL_Rect destination = {.x = x, .y = y};
    SDL_QueryTexture(texture, NULL, NULL, &destination.w, &destination.h);
    destination.w *= x_scale;
    destination.h *= y_scale;
    SDL_RenderCopyEx(renderer, texture, NULL, &destination, angle, NULL, SDL_FLIP_NONE);
}

void gfx_init(int new_window_width, int new_window_height, int new_tile_size)
{
    window_width = new_window_width;
    window_height = new_window_height;
    tile_size = new_tile_size;
    char * title = "Test Window";

    TTF_Init();
    SDL_VideoInit(NULL);
    mainWindow = SDL_CreateWindow(title, 0, 0, window_width*tile_size, window_height*tile_size, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    load_texture("bg.bmp", &texture_bg);
    load_texture("head.bmp", &texture_head);
    load_texture("tail_straight.bmp", &texture_tail_straight);
    load_texture("tail_corner.bmp", &texture_tail_corner);
    load_texture("tail_end.bmp", &texture_tail_end);
    load_texture("apple.bmp", &texture_apple);
    load_texture("border.bmp", &texture_border);

    font_sans = TTF_OpenFont("OpenSans-Regular.ttf", 48);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture_bg, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_RaiseWindow(mainWindow);
}

void gfx_exit(void)
{
    SDL_DestroyTexture(texture_bg);
    SDL_DestroyTexture(texture_head);
    SDL_DestroyTexture(texture_tail_straight);
    SDL_DestroyTexture(texture_tail_corner);
    SDL_DestroyTexture(texture_apple);
    SDL_DestroyTexture(texture_border);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainWindow);
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
        case GFX_TAIL_STRAIGHT:
            texture = texture_tail_straight;
            break;
        case GFX_TAIL_END:
            texture = texture_tail_end;
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

void gfx_draw_ex(GfxElement_t element, int x, int y, float x_scale, float y_scale, double angle)
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
        case GFX_TAIL_STRAIGHT:
            texture = texture_tail_straight;
            break;
        case GFX_TAIL_CORNER:
            texture = texture_tail_corner;
            break;
        case GFX_TAIL_END:
            texture = texture_tail_end;
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

    render_texture_ex(texture, x * tile_size + (tile_size - x_scale * tile_size), y * tile_size + (tile_size - y_scale * tile_size), x_scale, y_scale, angle);
}

void gfx_clear(void)
{
    SDL_RenderClear(renderer);
}

void gfx_present(void)
{
    SDL_RenderPresent(renderer);
}

void render_text(const char* text)
{
    SDL_Surface *textSurface = TTF_RenderText_Solid(font_sans, text, WHITE);
    text_texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect text_rect = 
    {
        128, 16, 256, 64
    };
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
}

