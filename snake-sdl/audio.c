#include "audio.h"

Mix_Chunk *clip_music;
Mix_Chunk *clip_pickup;
Mix_Chunk *clip_applespawn;
Mix_Chunk *clip_gameover;

void audio_init(void)
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 4, 256);

    clip_music = Mix_LoadWAV("music01.wav");

    clip_pickup = Mix_LoadWAV("sfx01.wav");
    clip_applespawn = Mix_LoadWAV("sfx02.wav");
    clip_gameover = Mix_LoadWAV("sfx07.wav");

    Mix_PlayChannel(-1, clip_music, -1);
}

void audio_exit(void)
{
    Mix_Quit();
}

void audio_play_sfx(SFXID_t sfxId)
{
    Mix_Chunk *clip;

    switch (sfxId) 
    {
    case SFX_NONE:
        return;
    case SFX_PICKUP:
        clip = clip_pickup;
        break;
    case SFX_APPLE_SPAWN:
        clip = clip_applespawn;
        break;
    case SFX_GAME_OVER:
        clip = clip_gameover;
        break;
    }

    Mix_PlayChannel(-1, clip, 0);
}
