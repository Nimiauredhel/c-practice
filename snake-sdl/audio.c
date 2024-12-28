#include "audio.h"

Mix_Chunk *clip_music;
Mix_Chunk *clip_sfx;

void audio_init(void)
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 4, 256);
    clip_music = Mix_LoadWAV("music01.wav");
    clip_sfx = Mix_LoadWAV("sfx01.wav");
    Mix_PlayChannel(-1, clip_music, -1);
}

void audio_exit(void)
{
    Mix_Quit();
}

void audio_play_sfx(void)
{
    Mix_PlayChannel(-1, clip_sfx, 0);
}
