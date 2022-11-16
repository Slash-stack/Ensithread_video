#include "synchro.h"
#include "ensitheora.h"
#include <semaphore.h>

extern bool fini;

/* les variables pour la synchro, ici */
pthread_mutex_t hashmap_mutex = PTHREAD_MUTEX_INITIALIZER;

bool taille_fenetre_prete = false;
bool fenetre_texture_prete = false;
int cpt = 0;

pthread_mutex_t taille_fenetre_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fenetre_texture_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t texture_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t taille_fenetre_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t fenetre_texture_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t texture_cond = PTHREAD_COND_INITIALIZER;


/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer)
{
    pthread_mutex_lock(&taille_fenetre_mutex);
    windowsx = buffer[0].width;
    windowsy = buffer[0].height;
    taille_fenetre_prete = true;
    pthread_cond_broadcast(&taille_fenetre_cond);
    pthread_mutex_unlock(&taille_fenetre_mutex);
}

void attendreTailleFenetre()
{
    pthread_mutex_lock(&taille_fenetre_mutex);
    while (!taille_fenetre_prete) {
        pthread_cond_wait(&taille_fenetre_cond, &taille_fenetre_mutex);
    }
    pthread_mutex_unlock(&taille_fenetre_mutex);
}

void signalerFenetreEtTexturePrete()
{
    pthread_mutex_lock(&fenetre_texture_mutex);
    fenetre_texture_prete = true;
    pthread_cond_broadcast(&fenetre_texture_cond);
    pthread_mutex_unlock(&fenetre_texture_mutex);
}

void attendreFenetreTexture()
{

    pthread_mutex_lock(&fenetre_texture_mutex);
    while (!fenetre_texture_prete) {
        pthread_cond_wait(&fenetre_texture_cond, &fenetre_texture_mutex);
    }
    pthread_mutex_unlock(&fenetre_texture_mutex);
}

void debutConsommerTexture()
{
    pthread_mutex_lock(&texture_mutex);
    while (cpt <= 0)
    {
        pthread_cond_wait(&texture_cond, &texture_mutex);
    }
    cpt--;
}

void finConsommerTexture()
{
    pthread_cond_signal(&texture_cond);
    pthread_mutex_unlock(&texture_mutex);
}

void debutDeposerTexture()
{
    pthread_mutex_lock(&texture_mutex);
    while (cpt >= NBTEX)
    {
        pthread_cond_wait(&texture_cond, &texture_mutex);
    }
    cpt++;
}

void finDeposerTexture()
{
    pthread_cond_signal(&texture_cond);
    pthread_mutex_unlock(&texture_mutex);
}