#include "gfx.h"

SDL_Window *mainWindow;
SDL_Renderer *renderer;

SDL_Texture *texture_bg;
SDL_Texture *texture_head;
SDL_Texture *texture_tail;
SDL_Texture *texture_apple;
SDL_Texture *texture_border;

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

void gfx_init(void)
{
    char * title = "Test Window";
    SDL_Init(SDL_INIT_VIDEO);
    mainWindow = SDL_CreateWindow(title, 0, 0, 1280, 720, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    load_texture("sample.bmp", &texture_bg);
    load_texture("head.bmp", &texture_head);
    load_texture("tail.bmp", &texture_tail);
    load_texture("apple.bmp", &texture_apple);
    load_texture("border.bmp", &texture_border);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture_bg, NULL, NULL);
    SDL_RenderPresent(renderer);
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
            return;
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

    render_texture(texture, x * 32, y * 32);
}

void gfx_clear(void)
{
    SDL_RenderClear(renderer);
}

void gfx_present(void)
{
    SDL_RenderPresent(renderer);
}
