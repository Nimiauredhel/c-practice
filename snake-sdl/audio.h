#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

void audio_init(void);
void audio_exit(void);
void audio_play_sfx(void);

#endif
