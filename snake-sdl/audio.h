#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

typedef enum SFXID
{
    SFX_NONE = 0,
    SFX_PICKUP = 1,
    SFX_APPLE_SPAWN = 2,
    SFX_GAME_OVER = 3,
} SFXID_t;

void audio_init(void);
void audio_exit(void);
void audio_play_sfx(SFXID_t);

#endif
