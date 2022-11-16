#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include "pthread.h"
#include "synchro.h"
#include "stream_common.h"
#include "oggstream.h"


int main(int argc, char *argv[]) {
    int res;

    if (argc != 2) {
	fprintf(stderr, "Usage: %s FILE", argv[0]);
	exit(EXIT_FAILURE);
    }
    assert(argc == 2);


    // Initialisation de la SDL
    res = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS);
    atexit(SDL_Quit);
    assert(res == 0);
    
    // start the two stream readers
    pthread_t vorbis_audio_thread;
    pthread_t theora_video_thread;

    if (pthread_create(&vorbis_audio_thread, NULL, vorbisStreamReader, argv[1]) != 0)
    {
        exit(-1);
    }
    if (pthread_create(&theora_video_thread, NULL, theoraStreamReader, argv[1]) != 0)
    {
        exit(-1);
    }
    
    // wait audio thread
    fflush(stdout);
    pthread_join(vorbis_audio_thread, NULL);

    // 1 seconde de garde pour le son,
    sleep(1);

    // tuer les deux threads videos si ils sont bloqués
    // TODO : il faut rajouter la verification de si les threads sont bloqués
    pthread_cancel(theora_video_thread);
    pthread_cancel(theora2sdlthread);

    // attendre les 2 threads videos
    pthread_join(theora_video_thread, NULL);
    pthread_join(theora2sdlthread, NULL);
    
    exit(EXIT_SUCCESS);    
}
