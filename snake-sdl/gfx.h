#ifndef GFX_H
#define GFX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

typedef enum GfxElement
{
    GFX_NONE = 0,
    GFX_HEAD = 1,
    GFX_TAIL = 2,
    GFX_APPLE = 3,
    GFX_BORDER = 4
} GfxElement_t;

void gfx_init(void);
void gfx_exit(void);
void gfx_draw(GfxElement_t element, int x, int y);
void gfx_clear(void);
void gfx_present(void);

#endif
