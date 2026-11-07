#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#define TAM_TEXTO 100

typedef enum { PRETO, VERMELHO } Cor;

typedef struct musica {
    char titulo[TAM_TEXTO];
    double minutos;
    Cor cor;
    struct musica *esq;
    struct musica *dir;
} Musica;

typedef struct album {
    char titulo[TAM_TEXTO];
    int ano;
    int quantidadeMusicas;
    Musica *musicas;
    Cor cor;
    struct album *esq;
    struct album *dir;
} Album;

typedef struct artista {
    char nome[TAM_TEXTO];
    char tipo[TAM_TEXTO];
    char estilo[TAM_TEXTO];
    int numeroAlbuns;
    Album *albuns;
    Cor cor;
    struct artista *esq;
    struct artista *dir;
} Artista;

typedef struct musicaPlaylist {
    char artista[TAM_TEXTO];
    char album[TAM_TEXTO];
    char titulo[TAM_TEXTO];
    Cor cor;
    struct musicaPlaylist *esq;
    struct musicaPlaylist *dir;
} MusicaPlaylist;

typedef struct playlist {
    char nome[TAM_TEXTO];
    MusicaPlaylist *musicas;
    Cor cor;
    struct playlist *esq;
    struct playlist *dir;
} Playlist;
