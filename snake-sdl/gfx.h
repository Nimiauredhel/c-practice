#ifndef GFX_H
#define GFX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>

typedef enum GfxElement
{
    GFX_NONE = 0,
    GFX_HEAD = 1,
    GFX_TAIL_STRAIGHT = 2,
    GFX_TAIL_CORNER = 3,
    GFX_APPLE = 4,
    GFX_BORDER = 5
} GfxElement_t;

void gfx_init(int new_window_width, int new_window_height, int new_tile_size);
void gfx_exit(void);
void gfx_draw(GfxElement_t element, int x, int y);
void gfx_draw_ex(GfxElement_t element, int x, int y, float x_scale, float y_scale, double angle);
void gfx_clear(void);
void gfx_present(void);
void render_text(const char* text);

#endif
