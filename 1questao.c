#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#define TAM_TEXTO 100

typedef struct musica {
    char titulo[TAM_TEXTO];
    double minutos;
    struct musica *esq;
    struct musica *dir;
} Musica;

typedef struct album {
    char titulo[TAM_TEXTO];
    int ano;
    int quantidadeMusicas;
    Musica *musicas;
    struct album *esq;
    struct album *dir;
} Album;

typedef struct artista {
    char nome[TAM_TEXTO];
    char tipo[TAM_TEXTO];
    char estilo[TAM_TEXTO];
    int numeroAlbuns;
    Album *albuns;
    struct artista *esq;
    struct artista *dir;
} Artista;

typedef struct musicaPlaylist {
    char artista[TAM_TEXTO];
    char album[TAM_TEXTO];
    char titulo[TAM_TEXTO];
    struct musicaPlaylist *esq;
    struct musicaPlaylist *dir;
} MusicaPlaylist;

typedef struct playlist {
    char nome[TAM_TEXTO];
    MusicaPlaylist *musicas;
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

int inserirArtista(Artista **raiz, const char nome[], const char tipo[],
                   const char estilo[]) {
    int comparacao;
    Artista *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof(Artista));
        if (novo == NULL)
            return -1;
        strcpy(novo->nome, nome);
        strcpy(novo->tipo, tipo);
        strcpy(novo->estilo, estilo);
        novo->numeroAlbuns = 0;
        novo->albuns = NULL;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }
    comparacao = strcmp(nome, (*raiz)->nome);
    if (comparacao < 0)
        return inserirArtista(&(*raiz)->esq, nome, tipo, estilo);
    if (comparacao > 0)
        return inserirArtista(&(*raiz)->dir, nome, tipo, estilo);
    return 0;
}

int inserirAlbum(Album **raiz, const char titulo[], int ano) {
    int comparacao;
    Album *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof(Album));
        if (novo == NULL)
            return -1;
        strcpy(novo->titulo, titulo);
        novo->ano = ano;
        novo->quantidadeMusicas = 0;
        novo->musicas = NULL;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }
    comparacao = strcmp(titulo, (*raiz)->titulo);
    if (comparacao < 0)
        return inserirAlbum(&(*raiz)->esq, titulo, ano);
    if (comparacao > 0)
        return inserirAlbum(&(*raiz)->dir, titulo, ano);
    return 0;
}

int inserirMusica(Musica **raiz, const char titulo[], double minutos) {
    int comparacao;
    Musica *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof(Musica));
        if (novo == NULL)
            return -1;
        strcpy(novo->titulo, titulo);
        novo->minutos = minutos;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }
    comparacao = strcmp(titulo, (*raiz)->titulo);
    if (comparacao < 0)
        return inserirMusica(&(*raiz)->esq, titulo, minutos);
    if (comparacao > 0)
        return inserirMusica(&(*raiz)->dir, titulo, minutos);
    return 0;
}

int inserirPlaylist(Playlist **raiz, const char nome[]) {
    int comparacao;
    Playlist *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof(Playlist));
        if (novo == NULL)
            return -1;
        strcpy(novo->nome, nome);
        novo->musicas = NULL;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }
    comparacao = strcmp(nome, (*raiz)->nome);
    if (comparacao < 0)
        return inserirPlaylist(&(*raiz)->esq, nome);
    if (comparacao > 0)
        return inserirPlaylist(&(*raiz)->dir, nome);
    return 0;
}

int inserirMusicaPlaylist(MusicaPlaylist **raiz, const char artista[],
                         const char album[], const char titulo[]) {
    int comparacao;
    MusicaPlaylist *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof(MusicaPlaylist));
        if (novo == NULL)
            return -1;
        strcpy(novo->artista, artista);
        strcpy(novo->album, album);
        strcpy(novo->titulo, titulo);
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }
    comparacao = compararMusicaPlaylist(artista, album, titulo, *raiz);
    if (comparacao < 0)
        return inserirMusicaPlaylist(&(*raiz)->esq, artista, album, titulo);
    if (comparacao > 0)
        return inserirMusicaPlaylist(&(*raiz)->dir, artista, album, titulo);
    return 0;
}

int removerMusica(Musica **raiz, const char titulo[]) {
    int comparacao;
    Musica *removido;
    Musica *menor;

    if (*raiz == NULL)
        return 0;
    comparacao = strcmp(titulo, (*raiz)->titulo);
    if (comparacao < 0)
        return removerMusica(&(*raiz)->esq, titulo);
    if (comparacao > 0)
        return removerMusica(&(*raiz)->dir, titulo);

    if ((*raiz)->esq == NULL || (*raiz)->dir == NULL) {
        removido = *raiz;
        if (removido->esq != NULL)
            *raiz = removido->esq;
        else
            *raiz = removido->dir;
        free(removido);
        return 1;
    }

    menor = (*raiz)->dir;
    while (menor->esq != NULL)
        menor = menor->esq;
    strcpy((*raiz)->titulo, menor->titulo);
    (*raiz)->minutos = menor->minutos;
    return removerMusica(&(*raiz)->dir, menor->titulo);
}

int removerMusicaPlaylist(MusicaPlaylist **raiz, const char artista[],
                         const char album[], const char titulo[]) {
    int comparacao;
    MusicaPlaylist *removido;
    MusicaPlaylist *menor;

    if (*raiz == NULL)
        return 0;
    comparacao = compararMusicaPlaylist(artista, album, titulo, *raiz);
    if (comparacao < 0)
        return removerMusicaPlaylist(&(*raiz)->esq, artista, album, titulo);
    if (comparacao > 0)
        return removerMusicaPlaylist(&(*raiz)->dir, artista, album, titulo);

    if ((*raiz)->esq == NULL || (*raiz)->dir == NULL) {
        removido = *raiz;
        if (removido->esq != NULL)
            *raiz = removido->esq;
        else
            *raiz = removido->dir;
        free(removido);
        return 1;
    }

    menor = (*raiz)->dir;
    while (menor->esq != NULL)
        menor = menor->esq;
    strcpy((*raiz)->artista, menor->artista);
    strcpy((*raiz)->album, menor->album);
    strcpy((*raiz)->titulo, menor->titulo);
    return removerMusicaPlaylist(&(*raiz)->dir, menor->artista,
                                 menor->album, menor->titulo);
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
    if (buscarMusica(album->musicas, tituloMusica) == NULL) {
        printf("Musica nao encontrada neste album.\n");
        return 1;
    }
    if (musicaEmAlgumaPlaylist(playlists, artista->nome, album->titulo, tituloMusica)) {
        printf("Remocao bloqueada: a musica pertence a pelo menos uma playlist.\n");
        return 1;
    }
    if (removerMusica(&album->musicas, tituloMusica)) {
        album->quantidadeMusicas--;
        printf("Musica removida do album.\n");
    }
    return 1;
}

void mostrarMenu(void) {
    printf("\n========== BIBLIOTECA DE MUSICAS - ABB ==========\n");
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

    printf("Biblioteca de musicas - Questao 1\n");
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
