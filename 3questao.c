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

int lerLinha(char texto[], int tamanho) {
    int caractere;
    size_t fim;

    if (fgets(texto, tamanho, stdin) == NULL)
        return 0;

    if (strchr(texto, '\n') == NULL) {
        caractere = getchar();
        if (caractere != '\n' && caractere != EOF) {
            while (caractere != '\n' && caractere != EOF)
                caractere = getchar();
            return -1;
        }
    }

    fim = strcspn(texto, "\r\n");
    texto[fim] = '\0';
    return 1;
}

void apararEspacos(char texto[]) {
    size_t inicio = 0;
    size_t fim = strlen(texto);

    while (inicio < fim && isspace((unsigned char)texto[inicio]))
        inicio++;
    while (fim > inicio && isspace((unsigned char)texto[fim - 1]))
        fim--;

    memmove(texto, texto + inicio, fim - inicio);
    texto[fim - inicio] = '\0';
}

int lerTexto(const char mensagem[], char texto[], int tamanho) {
    int resultado;

    while (1) {
        printf("%s", mensagem);
        fflush(stdout);
        resultado = lerLinha(texto, tamanho);
        if (resultado == 0)
            return 0;
        if (resultado == -1) {
            printf("Texto muito longo. Use no maximo %d bytes.\n", tamanho - 1);
        } else {
            apararEspacos(texto);
            if (texto[0] != '\0')
                return 1;
            printf("O campo nao pode ficar vazio.\n");
        }
    }
}

int lerInteiro(const char mensagem[], int minimo, int maximo, int *valor) {
    char texto[128];
    char *fim;
    long numero;

    while (1) {
        if (!lerTexto(mensagem, texto, (int)sizeof(texto)))
            return 0;
        errno = 0;
        numero = strtol(texto, &fim, 10);
        if (errno == 0 && fim != texto && *fim == '\0' &&
            numero >= minimo && numero <= maximo) {
            *valor = (int)numero;
            return 1;
        }
        printf("Digite um numero inteiro entre %d e %d.\n", minimo, maximo);
    }
}

int lerDuracao(double *minutos) {
    char texto[128];
    char *fim;
    double numero;
    int i;

    while (1) {
        if (!lerTexto("Duracao em minutos (ex.: 3.5 ou 3,5): ",
                      texto, (int)sizeof(texto)))
            return 0;
        for (i = 0; texto[i] != '\0'; i++) {
            if (texto[i] == ',')
                texto[i] = '.';
        }
        errno = 0;
        numero = strtod(texto, &fim);
        if (errno == 0 && fim != texto && *fim == '\0' &&
            isfinite(numero) && numero > 0.0) {
            *minutos = numero;
            return 1;
        }
        printf("Digite uma duracao positiva e finita.\n");
    }
}

Artista *buscarArtista(Artista *raiz, const char nome[]) {
    int comparacao;

    if (raiz == NULL)
        return NULL;
    comparacao = strcmp(nome, raiz->nome);
    if (comparacao < 0)
        return buscarArtista(raiz->esq, nome);
    if (comparacao > 0)
        return buscarArtista(raiz->dir, nome);
    return raiz;
}

Album *buscarAlbum(Album *raiz, const char titulo[]) {
    int comparacao;

    if (raiz == NULL)
        return NULL;
    comparacao = strcmp(titulo, raiz->titulo);
    if (comparacao < 0)
        return buscarAlbum(raiz->esq, titulo);
    if (comparacao > 0)
        return buscarAlbum(raiz->dir, titulo);
    return raiz;
}

Musica *buscarMusica(Musica *raiz, const char titulo[]) {
    int comparacao;

    if (raiz == NULL)
        return NULL;
    comparacao = strcmp(titulo, raiz->titulo);
    if (comparacao < 0)
        return buscarMusica(raiz->esq, titulo);
    if (comparacao > 0)
        return buscarMusica(raiz->dir, titulo);
    return raiz;
}

Playlist *buscarPlaylist(Playlist *raiz, const char nome[]) {
    int comparacao;

    if (raiz == NULL)
        return NULL;
    comparacao = strcmp(nome, raiz->nome);
    if (comparacao < 0)
        return buscarPlaylist(raiz->esq, nome);
    if (comparacao > 0)
        return buscarPlaylist(raiz->dir, nome);
    return raiz;
}

int compararMusicaPlaylist(const char artista[], const char album[],
                          const char titulo[], const MusicaPlaylist *musica) {
    int comparacao = strcmp(titulo, musica->titulo);

    if (comparacao == 0)
        comparacao = strcmp(artista, musica->artista);
    if (comparacao == 0)
        comparacao = strcmp(album, musica->album);
    return comparacao;
}

MusicaPlaylist *buscarMusicaPlaylist(MusicaPlaylist *raiz,
                                     const char artista[], const char album[],
                                     const char titulo[]) {
    int comparacao;

    if (raiz == NULL)
        return NULL;
    comparacao = compararMusicaPlaylist(artista, album, titulo, raiz);
    if (comparacao < 0)
        return buscarMusicaPlaylist(raiz->esq, artista, album, titulo);
    if (comparacao > 0)
        return buscarMusicaPlaylist(raiz->dir, artista, album, titulo);
    return raiz;
}

static Cor corOposta(Cor cor) {
    return cor == PRETO ? VERMELHO : PRETO;
}

static Cor corArtista(const Artista *no) {
    return no == NULL ? PRETO : no->cor;
}

static Artista *rotacionarEsquerdaArtista(Artista *raiz) {
    Artista *novaRaiz = raiz->dir;
    raiz->dir = novaRaiz->esq;
    novaRaiz->esq = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static Artista *rotacionarDireitaArtista(Artista *raiz) {
    Artista *novaRaiz = raiz->esq;
    raiz->esq = novaRaiz->dir;
    novaRaiz->dir = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static void trocarCoresArtista(Artista *raiz) {
    raiz->cor = corOposta(raiz->cor);
    raiz->esq->cor = corOposta(raiz->esq->cor);
    raiz->dir->cor = corOposta(raiz->dir->cor);
}

static Artista *balancearArtista(Artista *raiz) {
    /* Cada ajuste pode criar a condicao que o proximo teste verifica. */
    if (corArtista(raiz->dir) == VERMELHO &&
        corArtista(raiz->esq) == PRETO)
        raiz = rotacionarEsquerdaArtista(raiz);
    if (corArtista(raiz->esq) == VERMELHO &&
        corArtista(raiz->esq->esq) == VERMELHO)
        raiz = rotacionarDireitaArtista(raiz);
    if (corArtista(raiz->esq) == VERMELHO &&
        corArtista(raiz->dir) == VERMELHO)
        trocarCoresArtista(raiz);
    return raiz;
}

static int inserirArtistaRec(Artista **raiz, const char nome[], const char tipo[],
                             const char estilo[]) {
    int comparacao;
    int resultado;
    Artista *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof *novo);
        if (novo == NULL)
            return -1;
        strcpy(novo->nome, nome);
        strcpy(novo->tipo, tipo);
        strcpy(novo->estilo, estilo);
        novo->numeroAlbuns = 0;
        novo->albuns = NULL;
        novo->cor = VERMELHO;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }

    comparacao = strcmp(nome, (*raiz)->nome);
    if (comparacao < 0)
        resultado = inserirArtistaRec(&(*raiz)->esq, nome, tipo, estilo);
    else if (comparacao > 0)
        resultado = inserirArtistaRec(&(*raiz)->dir, nome, tipo, estilo);
    else
        return 0;

    if (resultado == 1)
        *raiz = balancearArtista(*raiz);
    return resultado;
}

int inserirArtista(Artista **raiz, const char nome[], const char tipo[],
                   const char estilo[]) {
    int resultado = inserirArtistaRec(raiz, nome, tipo, estilo);
    if (resultado == 1)
        (*raiz)->cor = PRETO;
    return resultado;
}

static Cor corAlbum(const Album *no) {
    return no == NULL ? PRETO : no->cor;
}

static Album *rotacionarEsquerdaAlbum(Album *raiz) {
    Album *novaRaiz = raiz->dir;
    raiz->dir = novaRaiz->esq;
    novaRaiz->esq = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static Album *rotacionarDireitaAlbum(Album *raiz) {
    Album *novaRaiz = raiz->esq;
    raiz->esq = novaRaiz->dir;
    novaRaiz->dir = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static void trocarCoresAlbum(Album *raiz) {
    raiz->cor = corOposta(raiz->cor);
    raiz->esq->cor = corOposta(raiz->esq->cor);
    raiz->dir->cor = corOposta(raiz->dir->cor);
}

static Album *balancearAlbum(Album *raiz) {
    if (corAlbum(raiz->dir) == VERMELHO &&
        corAlbum(raiz->esq) == PRETO)
        raiz = rotacionarEsquerdaAlbum(raiz);
    if (corAlbum(raiz->esq) == VERMELHO &&
        corAlbum(raiz->esq->esq) == VERMELHO)
        raiz = rotacionarDireitaAlbum(raiz);
    if (corAlbum(raiz->esq) == VERMELHO &&
        corAlbum(raiz->dir) == VERMELHO)
        trocarCoresAlbum(raiz);
    return raiz;
}

static int inserirAlbumRec(Album **raiz, const char titulo[], int ano) {
    int comparacao;
    int resultado;
    Album *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof *novo);
        if (novo == NULL)
            return -1;
        strcpy(novo->titulo, titulo);
        novo->ano = ano;
        novo->quantidadeMusicas = 0;
        novo->musicas = NULL;
        novo->cor = VERMELHO;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }

    comparacao = strcmp(titulo, (*raiz)->titulo);
    if (comparacao < 0)
        resultado = inserirAlbumRec(&(*raiz)->esq, titulo, ano);
    else if (comparacao > 0)
        resultado = inserirAlbumRec(&(*raiz)->dir, titulo, ano);
    else
        return 0;

    if (resultado == 1)
        *raiz = balancearAlbum(*raiz);
    return resultado;
}

int inserirAlbum(Album **raiz, const char titulo[], int ano) {
    int resultado = inserirAlbumRec(raiz, titulo, ano);
    if (resultado == 1)
        (*raiz)->cor = PRETO;
    return resultado;
}

static Cor corMusica(const Musica *no) {
    return no == NULL ? PRETO : no->cor;
}

static Musica *rotacionarEsquerdaMusica(Musica *raiz) {
    Musica *novaRaiz = raiz->dir;
    raiz->dir = novaRaiz->esq;
    novaRaiz->esq = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static Musica *rotacionarDireitaMusica(Musica *raiz) {
    Musica *novaRaiz = raiz->esq;
    raiz->esq = novaRaiz->dir;
    novaRaiz->dir = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static void trocarCoresMusica(Musica *raiz) {
    raiz->cor = corOposta(raiz->cor);
    raiz->esq->cor = corOposta(raiz->esq->cor);
    raiz->dir->cor = corOposta(raiz->dir->cor);
}

static Musica *balancearMusica(Musica *raiz) {
    if (corMusica(raiz->dir) == VERMELHO &&
        corMusica(raiz->esq) == PRETO)
        raiz = rotacionarEsquerdaMusica(raiz);
    if (corMusica(raiz->esq) == VERMELHO &&
        corMusica(raiz->esq->esq) == VERMELHO)
        raiz = rotacionarDireitaMusica(raiz);
    if (corMusica(raiz->esq) == VERMELHO &&
        corMusica(raiz->dir) == VERMELHO)
        trocarCoresMusica(raiz);
    return raiz;
}

static int inserirMusicaRec(Musica **raiz, const char titulo[], double minutos) {
    int comparacao;
    int resultado;
    Musica *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof *novo);
        if (novo == NULL)
            return -1;
        strcpy(novo->titulo, titulo);
        novo->minutos = minutos;
        novo->cor = VERMELHO;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }

    comparacao = strcmp(titulo, (*raiz)->titulo);
    if (comparacao < 0)
        resultado = inserirMusicaRec(&(*raiz)->esq, titulo, minutos);
    else if (comparacao > 0)
        resultado = inserirMusicaRec(&(*raiz)->dir, titulo, minutos);
    else
        return 0;

    if (resultado == 1)
        *raiz = balancearMusica(*raiz);
    return resultado;
}

int inserirMusica(Musica **raiz, const char titulo[], double minutos) {
    int resultado = inserirMusicaRec(raiz, titulo, minutos);
    if (resultado == 1)
        (*raiz)->cor = PRETO;
    return resultado;
}

static Cor corPlaylist(const Playlist *no) {
    return no == NULL ? PRETO : no->cor;
}

static Playlist *rotacionarEsquerdaPlaylist(Playlist *raiz) {
    Playlist *novaRaiz = raiz->dir;
    raiz->dir = novaRaiz->esq;
    novaRaiz->esq = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static Playlist *rotacionarDireitaPlaylist(Playlist *raiz) {
    Playlist *novaRaiz = raiz->esq;
    raiz->esq = novaRaiz->dir;
    novaRaiz->dir = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static void trocarCoresPlaylist(Playlist *raiz) {
    raiz->cor = corOposta(raiz->cor);
    raiz->esq->cor = corOposta(raiz->esq->cor);
    raiz->dir->cor = corOposta(raiz->dir->cor);
}

static Playlist *balancearPlaylist(Playlist *raiz) {
    if (corPlaylist(raiz->dir) == VERMELHO &&
        corPlaylist(raiz->esq) == PRETO)
        raiz = rotacionarEsquerdaPlaylist(raiz);
    if (corPlaylist(raiz->esq) == VERMELHO &&
        corPlaylist(raiz->esq->esq) == VERMELHO)
        raiz = rotacionarDireitaPlaylist(raiz);
    if (corPlaylist(raiz->esq) == VERMELHO &&
        corPlaylist(raiz->dir) == VERMELHO)
        trocarCoresPlaylist(raiz);
    return raiz;
}

static int inserirPlaylistRec(Playlist **raiz, const char nome[]) {
    int comparacao;
    int resultado;
    Playlist *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof *novo);
        if (novo == NULL)
            return -1;
        strcpy(novo->nome, nome);
        novo->musicas = NULL;
        novo->cor = VERMELHO;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }

    comparacao = strcmp(nome, (*raiz)->nome);
    if (comparacao < 0)
        resultado = inserirPlaylistRec(&(*raiz)->esq, nome);
    else if (comparacao > 0)
        resultado = inserirPlaylistRec(&(*raiz)->dir, nome);
    else
        return 0;

    if (resultado == 1)
        *raiz = balancearPlaylist(*raiz);
    return resultado;
}

int inserirPlaylist(Playlist **raiz, const char nome[]) {
    int resultado = inserirPlaylistRec(raiz, nome);
    if (resultado == 1)
        (*raiz)->cor = PRETO;
    return resultado;
}

static Cor corMusicaPlaylist(const MusicaPlaylist *no) {
    return no == NULL ? PRETO : no->cor;
}

static MusicaPlaylist *rotacionarEsquerdaMusicaPlaylist(MusicaPlaylist *raiz) {
    MusicaPlaylist *novaRaiz = raiz->dir;
    raiz->dir = novaRaiz->esq;
    novaRaiz->esq = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static MusicaPlaylist *rotacionarDireitaMusicaPlaylist(MusicaPlaylist *raiz) {
    MusicaPlaylist *novaRaiz = raiz->esq;
    raiz->esq = novaRaiz->dir;
    novaRaiz->dir = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static void trocarCoresMusicaPlaylist(MusicaPlaylist *raiz) {
    raiz->cor = corOposta(raiz->cor);
    raiz->esq->cor = corOposta(raiz->esq->cor);
    raiz->dir->cor = corOposta(raiz->dir->cor);
}

static MusicaPlaylist *balancearMusicaPlaylist(MusicaPlaylist *raiz) {
    if (corMusicaPlaylist(raiz->dir) == VERMELHO &&
        corMusicaPlaylist(raiz->esq) == PRETO)
        raiz = rotacionarEsquerdaMusicaPlaylist(raiz);
    if (corMusicaPlaylist(raiz->esq) == VERMELHO &&
        corMusicaPlaylist(raiz->esq->esq) == VERMELHO)
        raiz = rotacionarDireitaMusicaPlaylist(raiz);
    if (corMusicaPlaylist(raiz->esq) == VERMELHO &&
        corMusicaPlaylist(raiz->dir) == VERMELHO)
        trocarCoresMusicaPlaylist(raiz);
    return raiz;
}

static int inserirMusicaPlaylistRec(MusicaPlaylist **raiz, const char artista[],
                                    const char album[], const char titulo[]) {
    int comparacao;
    int resultado;
    MusicaPlaylist *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof *novo);
        if (novo == NULL)
            return -1;
        strcpy(novo->artista, artista);
        strcpy(novo->album, album);
        strcpy(novo->titulo, titulo);
        novo->cor = VERMELHO;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }

    comparacao = compararMusicaPlaylist(artista, album, titulo, *raiz);
    if (comparacao < 0)
        resultado = inserirMusicaPlaylistRec(&(*raiz)->esq, artista, album, titulo);
    else if (comparacao > 0)
        resultado = inserirMusicaPlaylistRec(&(*raiz)->dir, artista, album, titulo);
    else
        return 0;

    if (resultado == 1)
        *raiz = balancearMusicaPlaylist(*raiz);
    return resultado;
}

int inserirMusicaPlaylist(MusicaPlaylist **raiz, const char artista[],
                          const char album[], const char titulo[]) {
    int resultado = inserirMusicaPlaylistRec(raiz, artista, album, titulo);
    if (resultado == 1)
        (*raiz)->cor = PRETO;
    return resultado;
}

/* A busca inicial garante que a chave existe. Na descida, movemos um elo
   vermelho para o lado da remocao quando preciso. Na volta, balanceamos. */
static Musica *moverVermelhoEsquerdaMusica(Musica *raiz) {
    trocarCoresMusica(raiz);
    if (corMusica(raiz->dir->esq) == VERMELHO) {
        raiz->dir = rotacionarDireitaMusica(raiz->dir);
        raiz = rotacionarEsquerdaMusica(raiz);
        trocarCoresMusica(raiz);
    }
    return raiz;
}

static Musica *moverVermelhoDireitaMusica(Musica *raiz) {
    trocarCoresMusica(raiz);
    if (corMusica(raiz->esq->esq) == VERMELHO) {
        raiz = rotacionarDireitaMusica(raiz);
        trocarCoresMusica(raiz);
    }
    return raiz;
}

static Musica *removerMenorMusica(Musica *raiz) {
    if (raiz->esq == NULL) {
        free(raiz);
        return NULL;
    }
    if (corMusica(raiz->esq) == PRETO &&
        corMusica(raiz->esq->esq) == PRETO)
        raiz = moverVermelhoEsquerdaMusica(raiz);

    raiz->esq = removerMenorMusica(raiz->esq);
    return balancearMusica(raiz);
}

static Musica *removerMusicaRec(Musica *raiz, const char titulo[]) {
    if (strcmp(titulo, raiz->titulo) < 0) {
        if (corMusica(raiz->esq) == PRETO &&
            corMusica(raiz->esq->esq) == PRETO)
            raiz = moverVermelhoEsquerdaMusica(raiz);
        raiz->esq = removerMusicaRec(raiz->esq, titulo);
    } else {
        if (corMusica(raiz->esq) == VERMELHO)
            raiz = rotacionarDireitaMusica(raiz);
        if (raiz->dir == NULL && strcmp(titulo, raiz->titulo) == 0) {
            free(raiz);
            return NULL;
        }

        if (corMusica(raiz->dir) == PRETO &&
            corMusica(raiz->dir->esq) == PRETO)
            raiz = moverVermelhoDireitaMusica(raiz);

        /* Uma rotacao pode ter mudado a raiz. Compare o titulo novamente. */
        if (strcmp(titulo, raiz->titulo) == 0) {
            Musica *sucessor = raiz->dir;
            while (sucessor->esq != NULL)
                sucessor = sucessor->esq;
            strcpy(raiz->titulo, sucessor->titulo);
            raiz->minutos = sucessor->minutos;
            raiz->dir = removerMenorMusica(raiz->dir);
        } else {
            raiz->dir = removerMusicaRec(raiz->dir, titulo);
        }
    }
    return balancearMusica(raiz);
}

int removerMusica(Musica **raiz, const char titulo[]) {
    if (buscarMusica(*raiz, titulo) == NULL)
        return 0;

    if (corMusica((*raiz)->esq) == PRETO &&
        corMusica((*raiz)->dir) == PRETO)
        (*raiz)->cor = VERMELHO;
    *raiz = removerMusicaRec(*raiz, titulo);
    if (*raiz != NULL)
        (*raiz)->cor = PRETO;
    return 1;
}

static MusicaPlaylist *moverVermelhoEsquerdaMusicaPlaylist(MusicaPlaylist *raiz) {
    trocarCoresMusicaPlaylist(raiz);
    if (corMusicaPlaylist(raiz->dir->esq) == VERMELHO) {
        raiz->dir = rotacionarDireitaMusicaPlaylist(raiz->dir);
        raiz = rotacionarEsquerdaMusicaPlaylist(raiz);
        trocarCoresMusicaPlaylist(raiz);
    }
    return raiz;
}

static MusicaPlaylist *moverVermelhoDireitaMusicaPlaylist(MusicaPlaylist *raiz) {
    trocarCoresMusicaPlaylist(raiz);
    if (corMusicaPlaylist(raiz->esq->esq) == VERMELHO) {
        raiz = rotacionarDireitaMusicaPlaylist(raiz);
        trocarCoresMusicaPlaylist(raiz);
    }
    return raiz;
}

static MusicaPlaylist *removerMenorMusicaPlaylist(MusicaPlaylist *raiz) {
    if (raiz->esq == NULL) {
        free(raiz);
        return NULL;
    }
    if (corMusicaPlaylist(raiz->esq) == PRETO &&
        corMusicaPlaylist(raiz->esq->esq) == PRETO)
        raiz = moverVermelhoEsquerdaMusicaPlaylist(raiz);

    raiz->esq = removerMenorMusicaPlaylist(raiz->esq);
    return balancearMusicaPlaylist(raiz);
}

static MusicaPlaylist *removerMusicaPlaylistRec(MusicaPlaylist *raiz,
                                                const char artista[],
                                                const char album[], const char titulo[]) {
    if (compararMusicaPlaylist(artista, album, titulo, raiz) < 0) {
        if (corMusicaPlaylist(raiz->esq) == PRETO &&
            corMusicaPlaylist(raiz->esq->esq) == PRETO)
            raiz = moverVermelhoEsquerdaMusicaPlaylist(raiz);
        raiz->esq = removerMusicaPlaylistRec(raiz->esq, artista, album, titulo);
    } else {
        if (corMusicaPlaylist(raiz->esq) == VERMELHO)
            raiz = rotacionarDireitaMusicaPlaylist(raiz);
        if (raiz->dir == NULL &&
            compararMusicaPlaylist(artista, album, titulo, raiz) == 0) {
            free(raiz);
            return NULL;
        }

        if (corMusicaPlaylist(raiz->dir) == PRETO &&
            corMusicaPlaylist(raiz->dir->esq) == PRETO)
            raiz = moverVermelhoDireitaMusicaPlaylist(raiz);

        if (compararMusicaPlaylist(artista, album, titulo, raiz) == 0) {
            MusicaPlaylist *sucessor = raiz->dir;
            while (sucessor->esq != NULL)
                sucessor = sucessor->esq;
            strcpy(raiz->titulo, sucessor->titulo);
            strcpy(raiz->artista, sucessor->artista);
            strcpy(raiz->album, sucessor->album);
            raiz->dir = removerMenorMusicaPlaylist(raiz->dir);
        } else {
            raiz->dir = removerMusicaPlaylistRec(raiz->dir, artista, album, titulo);
        }
    }
    return balancearMusicaPlaylist(raiz);
}

int removerMusicaPlaylist(MusicaPlaylist **raiz, const char artista[],
                          const char album[], const char titulo[]) {
    if (buscarMusicaPlaylist(*raiz, artista, album, titulo) == NULL)
        return 0;

    if (corMusicaPlaylist((*raiz)->esq) == PRETO &&
        corMusicaPlaylist((*raiz)->dir) == PRETO)
        (*raiz)->cor = VERMELHO;
    *raiz = removerMusicaPlaylistRec(*raiz, artista, album, titulo);
    if (*raiz != NULL)
        (*raiz)->cor = PRETO;
    return 1;
}

int musicaEmAlgumaPlaylist(Playlist *raiz, const char artista[],
                          const char album[], const char titulo[]) {
    if (raiz == NULL)
        return 0;
    if (buscarMusicaPlaylist(raiz->musicas, artista, album, titulo) != NULL)
        return 1;
    if (musicaEmAlgumaPlaylist(raiz->esq, artista, album, titulo))
        return 1;
    return musicaEmAlgumaPlaylist(raiz->dir, artista, album, titulo);
}

void mostrarArtistas(Artista *raiz) {
    if (raiz != NULL) {
        mostrarArtistas(raiz->esq);
        printf("\nArtista: %s\nTipo: %s\nEstilo: %s\nNumero de albuns: %d\n",
               raiz->nome, raiz->tipo, raiz->estilo, raiz->numeroAlbuns);
        mostrarArtistas(raiz->dir);
    }
}

int mostrarAlbunsPorAno(Album *raiz, int ano) {
    int quantidade = 0;

    if (raiz != NULL) {
        quantidade += mostrarAlbunsPorAno(raiz->esq, ano);
        if (raiz->ano == ano) {
            printf("\nAlbum: %s\nAno: %d\nQuantidade de musicas: %d\n",
                   raiz->titulo, raiz->ano, raiz->quantidadeMusicas);
            quantidade++;
        }
        quantidade += mostrarAlbunsPorAno(raiz->dir, ano);
    }
    return quantidade;
}

void mostrarDadosMusica(const Artista *artista, const Album *album,
                       const Musica *musica) {
    printf("\nMusica: %s\nDuracao: %.2f minutos\n", musica->titulo, musica->minutos);
    printf("Artista: %s\nTipo: %s\nEstilo: %s\n",
           artista->nome, artista->tipo, artista->estilo);
    printf("Album: %s\nAno: %d\n", album->titulo, album->ano);
}

int procurarMusicaNosAlbuns(Album *raiz, const Artista *artista,
                           const char titulo[]) {
    int quantidade = 0;
    Musica *musica;

    if (raiz != NULL) {
        quantidade += procurarMusicaNosAlbuns(raiz->esq, artista, titulo);
        musica = buscarMusica(raiz->musicas, titulo);
        if (musica != NULL) {
            mostrarDadosMusica(artista, raiz, musica);
            quantidade++;
        }
        quantidade += procurarMusicaNosAlbuns(raiz->dir, artista, titulo);
    }
    return quantidade;
}

int procurarMusicaNosArtistas(Artista *raiz, const char titulo[]) {
    int quantidade = 0;

    if (raiz != NULL) {
        quantidade += procurarMusicaNosArtistas(raiz->esq, titulo);
        quantidade += procurarMusicaNosAlbuns(raiz->albuns, raiz, titulo);
        quantidade += procurarMusicaNosArtistas(raiz->dir, titulo);
    }
    return quantidade;
}

int mostrarMusicasPlaylist(MusicaPlaylist *raiz, Artista *artistas) {
    int quantidade = 0;
    Artista *artista;
    Album *album;
    Musica *musica;

    if (raiz != NULL) {
        quantidade += mostrarMusicasPlaylist(raiz->esq, artistas);
        artista = buscarArtista(artistas, raiz->artista);
        album = artista != NULL ? buscarAlbum(artista->albuns, raiz->album) : NULL;
        musica = album != NULL ? buscarMusica(album->musicas, raiz->titulo) : NULL;
        if (musica != NULL) {
            mostrarDadosMusica(artista, album, musica);
        } else {
            printf("\nArtista: %s\nAlbum: %s\nMusica: %s\n",
                   raiz->artista, raiz->album, raiz->titulo);
            printf("Referencia nao encontrada no catalogo.\n");
        }
        quantidade++;
        quantidade += mostrarMusicasPlaylist(raiz->dir, artistas);
    }
    return quantidade;
}

void liberarMusicas(Musica *raiz) {
    if (raiz != NULL) {
        liberarMusicas(raiz->esq);
        liberarMusicas(raiz->dir);
        free(raiz);
    }
}

void liberarAlbuns(Album *raiz) {
    if (raiz != NULL) {
        liberarAlbuns(raiz->esq);
        liberarAlbuns(raiz->dir);
        liberarMusicas(raiz->musicas);
        free(raiz);
    }
}

void liberarArtistas(Artista *raiz) {
    if (raiz != NULL) {
        liberarArtistas(raiz->esq);
        liberarArtistas(raiz->dir);
        liberarAlbuns(raiz->albuns);
        free(raiz);
    }
}

void liberarMusicasPlaylist(MusicaPlaylist *raiz) {
    if (raiz != NULL) {
        liberarMusicasPlaylist(raiz->esq);
        liberarMusicasPlaylist(raiz->dir);
        free(raiz);
    }
}

void liberarPlaylists(Playlist *raiz) {
    if (raiz != NULL) {
        liberarPlaylists(raiz->esq);
        liberarPlaylists(raiz->dir);
        liberarMusicasPlaylist(raiz->musicas);
        free(raiz);
    }
}

void informarCadastro(int resultado) {
    if (resultado == 1)
        printf("Cadastro realizado com sucesso.\n");
    else if (resultado == 0)
        printf("Cadastro repetido: operacao nao realizada.\n");
    else
        printf("Memoria insuficiente. Cadastro nao realizado.\n");
}

int cadastrarArtista(Artista **raiz) {
    char nome[TAM_TEXTO];
    char tipo[TAM_TEXTO];
    char estilo[TAM_TEXTO];

    if (!lerTexto("Nome do artista: ", nome, TAM_TEXTO))
        return 0;
    if (buscarArtista(*raiz, nome) != NULL) {
        printf("Artista ja cadastrado.\n");
        return 1;
    }
    if (!lerTexto("Tipo (cantor, cantora, dupla, banda, grupo...): ", tipo, TAM_TEXTO) ||
        !lerTexto("Estilo musical: ", estilo, TAM_TEXTO))
        return 0;
    informarCadastro(inserirArtista(raiz, nome, tipo, estilo));
    return 1;
}

int cadastrarAlbum(Artista *raiz) {
    char nome[TAM_TEXTO];
    char titulo[TAM_TEXTO];
    Artista *artista;
    int ano;
    int resultado;

    if (!lerTexto("Nome do artista: ", nome, TAM_TEXTO))
        return 0;
    artista = buscarArtista(raiz, nome);
    if (artista == NULL) {
        printf("Artista nao encontrado.\n");
        return 1;
    }
    if (!lerTexto("Titulo do album: ", titulo, TAM_TEXTO))
        return 0;
    if (buscarAlbum(artista->albuns, titulo) != NULL) {
        printf("Album ja cadastrado para este artista.\n");
        return 1;
    }
    if (!lerInteiro("Ano de lancamento: ", 1, 9999, &ano))
        return 0;
    resultado = inserirAlbum(&artista->albuns, titulo, ano);
    if (resultado == 1)
        artista->numeroAlbuns++;
    informarCadastro(resultado);
    return 1;
}

int cadastrarMusica(Artista *raiz) {
    char nome[TAM_TEXTO];
    char tituloAlbum[TAM_TEXTO];
    char tituloMusica[TAM_TEXTO];
    Artista *artista;
    Album *album;
    double minutos;
    int resultado;

    if (!lerTexto("Nome do artista: ", nome, TAM_TEXTO))
        return 0;
    artista = buscarArtista(raiz, nome);
    if (artista == NULL) {
        printf("Artista nao encontrado.\n");
        return 1;
    }
    if (!lerTexto("Titulo do album: ", tituloAlbum, TAM_TEXTO))
        return 0;
    album = buscarAlbum(artista->albuns, tituloAlbum);
    if (album == NULL) {
        printf("Album nao encontrado para este artista.\n");
        return 1;
    }
    if (!lerTexto("Titulo da musica: ", tituloMusica, TAM_TEXTO))
        return 0;
    if (buscarMusica(album->musicas, tituloMusica) != NULL) {
        printf("Musica ja cadastrada neste album.\n");
        return 1;
    }
    if (!lerDuracao(&minutos))
        return 0;
    resultado = inserirMusica(&album->musicas, tituloMusica, minutos);
    if (resultado == 1)
        album->quantidadeMusicas++;
    informarCadastro(resultado);
    return 1;
}

int consultarAlbunsPorAno(Artista *raiz) {
    char nome[TAM_TEXTO];
    Artista *artista;
    int ano;

    if (!lerTexto("Nome do artista: ", nome, TAM_TEXTO))
        return 0;
    artista = buscarArtista(raiz, nome);
    if (artista == NULL) {
        printf("Artista nao encontrado.\n");
        return 1;
    }
    if (!lerInteiro("Ano desejado: ", 1, 9999, &ano))
        return 0;
    printf("\nAlbuns de %s em %d:\n", artista->nome, ano);
    if (mostrarAlbunsPorAno(artista->albuns, ano) == 0)
        printf("Nenhum album encontrado neste ano.\n");
    return 1;
}

int consultarMusicaPorTitulo(Artista *raiz) {
    char titulo[TAM_TEXTO];
    int quantidade;

    if (!lerTexto("Titulo da musica: ", titulo, TAM_TEXTO))
        return 0;
    quantidade = procurarMusicaNosArtistas(raiz, titulo);
    if (quantidade == 0)
        printf("Musica nao encontrada no catalogo.\n");
    else
        printf("\nTotal de ocorrencias: %d\n", quantidade);
    return 1;
}

int cadastrarPlaylist(Playlist **raiz) {
    char nome[TAM_TEXTO];
    int resultado;

    if (!lerTexto("Nome da playlist: ", nome, TAM_TEXTO))
        return 0;
    resultado = inserirPlaylist(raiz, nome);
    informarCadastro(resultado);
    if (resultado == 1)
        printf("Use a opcao 11 para adicionar musicas a esta playlist.\n");
    return 1;
}

int adicionarMusicaPlaylist(Playlist *playlists, Artista *artistas) {
    char nomePlaylist[TAM_TEXTO];
    char nomeArtista[TAM_TEXTO];
    char tituloAlbum[TAM_TEXTO];
    char tituloMusica[TAM_TEXTO];
    Playlist *playlist;
    Artista *artista;
    Album *album;
    Musica *musica;

    if (!lerTexto("Nome da playlist: ", nomePlaylist, TAM_TEXTO))
        return 0;
    playlist = buscarPlaylist(playlists, nomePlaylist);
    if (playlist == NULL) {
        printf("Playlist nao encontrada.\n");
        return 1;
    }
    if (!lerTexto("Nome do artista: ", nomeArtista, TAM_TEXTO))
        return 0;
    artista = buscarArtista(artistas, nomeArtista);
    if (artista == NULL) {
        printf("Artista nao encontrado.\n");
        return 1;
    }
    if (!lerTexto("Titulo do album: ", tituloAlbum, TAM_TEXTO))
        return 0;
    album = buscarAlbum(artista->albuns, tituloAlbum);
    if (album == NULL) {
        printf("Album nao encontrado para este artista.\n");
        return 1;
    }
    if (!lerTexto("Titulo da musica: ", tituloMusica, TAM_TEXTO))
        return 0;
    musica = buscarMusica(album->musicas, tituloMusica);
    if (musica == NULL) {
        printf("Musica nao encontrada neste album.\n");
        return 1;
    }
    informarCadastro(inserirMusicaPlaylist(&playlist->musicas, artista->nome,
                                           album->titulo, musica->titulo));
    return 1;
}

int consultarPlaylist(Playlist *playlists, Artista *artistas) {
    char nome[TAM_TEXTO];
    Playlist *playlist;
    int quantidade;

    if (!lerTexto("Nome da playlist: ", nome, TAM_TEXTO))
        return 0;
    playlist = buscarPlaylist(playlists, nome);
    if (playlist == NULL) {
        printf("Playlist nao encontrada.\n");
        return 1;
    }
    printf("\nPlaylist: %s\n", playlist->nome);
    quantidade = mostrarMusicasPlaylist(playlist->musicas, artistas);
    if (quantidade == 0)
        printf("Playlist vazia.\n");
    printf("\nTotal de musicas na playlist: %d\n", quantidade);
    return 1;
}

int excluirMusicaDaPlaylist(Playlist *playlists) {
    char nomePlaylist[TAM_TEXTO];
    char nomeArtista[TAM_TEXTO];
    char tituloAlbum[TAM_TEXTO];
    char tituloMusica[TAM_TEXTO];
    Playlist *playlist;

    if (!lerTexto("Nome da playlist: ", nomePlaylist, TAM_TEXTO))
        return 0;
    playlist = buscarPlaylist(playlists, nomePlaylist);
    if (playlist == NULL) {
        printf("Playlist nao encontrada.\n");
        return 1;
    }
    if (!lerTexto("Nome do artista: ", nomeArtista, TAM_TEXTO) ||
        !lerTexto("Titulo do album: ", tituloAlbum, TAM_TEXTO) ||
        !lerTexto("Titulo da musica: ", tituloMusica, TAM_TEXTO))
        return 0;
    if (removerMusicaPlaylist(&playlist->musicas, nomeArtista,
                             tituloAlbum, tituloMusica))
        printf("Musica removida da playlist. O catalogo foi mantido.\n");
    else
        printf("Esta musica nao esta nesta playlist.\n");
    return 1;
}

int excluirMusicaDoAlbum(Artista *artistas, Playlist *playlists) {
    char nomeArtista[TAM_TEXTO];
    char tituloAlbum[TAM_TEXTO];
    char tituloMusica[TAM_TEXTO];
    Artista *artista;
    Album *album;

    if (!lerTexto("Nome do artista: ", nomeArtista, TAM_TEXTO))
        return 0;
    artista = buscarArtista(artistas, nomeArtista);
    if (artista == NULL) {
        printf("Artista nao encontrado.\n");
        return 1;
    }
    if (!lerTexto("Titulo do album: ", tituloAlbum, TAM_TEXTO))
        return 0;
    album = buscarAlbum(artista->albuns, tituloAlbum);
    if (album == NULL) {
        printf("Album nao encontrado para este artista.\n");
        return 1;
    }
    if (!lerTexto("Titulo da musica: ", tituloMusica, TAM_TEXTO))
        return 0;
    if (musicaEmAlgumaPlaylist(playlists, artista->nome, album->titulo, tituloMusica)) {
        printf("Remocao bloqueada: a musica pertence a pelo menos uma playlist.\n");
    } else if (removerMusica(&album->musicas, tituloMusica)) {
        album->quantidadeMusicas--;
        printf("Musica removida do album.\n");
    } else {
        printf("Musica nao encontrada neste album.\n");
    }
    return 1;
}

void mostrarMenu(void) {
    printf("\n========== BIBLIOTECA DE MUSICAS - RUBRO-NEGRA ==========\n");
    printf("1  - Cadastrar artista\n");
    printf("2  - Cadastrar album de um artista\n");
    printf("3  - Cadastrar musica de um album\n");
    printf("4  - Mostrar todos os artistas\n");
    printf("5  - Mostrar albuns de um artista por ano\n");
    printf("6  - Buscar musica pelo titulo\n");
    printf("7  - Criar playlist\n");
    printf("8  - Mostrar todos os dados de uma playlist\n");
    printf("9  - Remover musica de uma playlist\n");
    printf("10 - Remover musica de um album\n");
    printf("11 - Adicionar musica cadastrada a uma playlist\n");
    printf("0  - Sair\n");
}

int main(void) {
    Artista *artistas = NULL;
    Playlist *playlists = NULL;
    int opcao;
    int continuar = 1;

    printf("Biblioteca de musicas - Questao 3 (rubro-negra pendente a esquerda)\n");
    printf("Nomes e titulos diferenciam maiusculas e minusculas.\n");
    printf("Os cadastros permanecem apenas durante esta execucao.\n");

    while (continuar) {
        mostrarMenu();
        if (!lerInteiro("Opcao: ", 0, 11, &opcao))
            break;
        switch (opcao) {
            case 1:
                continuar = cadastrarArtista(&artistas);
                break;
            case 2:
                continuar = cadastrarAlbum(artistas);
                break;
            case 3:
                continuar = cadastrarMusica(artistas);
                break;
            case 4:
                if (artistas == NULL)
                    printf("Nenhum artista cadastrado.\n");
                else
                    mostrarArtistas(artistas);
                break;
            case 5:
                continuar = consultarAlbunsPorAno(artistas);
                break;
            case 6:
                continuar = consultarMusicaPorTitulo(artistas);
                break;
            case 7:
                continuar = cadastrarPlaylist(&playlists);
                break;
            case 8:
                continuar = consultarPlaylist(playlists, artistas);
                break;
            case 9:
                continuar = excluirMusicaDaPlaylist(playlists);
                break;
            case 10:
                continuar = excluirMusicaDoAlbum(artistas, playlists);
                break;
            case 11:
                continuar = adicionarMusicaPlaylist(playlists, artistas);
                break;
            case 0:
                continuar = 0;
                break;
        }
    }

    liberarPlaylists(playlists);
    liberarArtistas(artistas);
    printf("\nPrograma encerrado.\n");
    return 0;
}
