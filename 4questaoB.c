#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define N_ARTISTAS 100
#define TAM_TEXTO 100
#define SEMENTE 2026U
#define ARQUIVO_CSV "questao4b_buscas.csv"

typedef enum { ABB, AVL, LLRB, N_ARVORES } TipoArvore;
typedef enum { CRESCENTE, DECRESCENTE, ALEATORIA, N_ORDENS } Ordem;
typedef enum { PRETO, VERMELHO } Cor;

static const char *const nomeArvore[N_ARVORES] = {"ABB", "AVL", "LLRB"};
static const char *const nomeOrdem[N_ORDENS] = {"crescente", "decrescente", "aleatoria"};

/* Posicoes contadas a partir de 1 no vetor original, em ordem de sorteio. */
static const int posicoesSorteio[] = {1, 12, 23, 34, 45, 56, 67, 78, 89, 100};
enum { N_BUSCAS = sizeof posicoesSorteio / sizeof posicoesSorteio[0] };

typedef struct {
    char texto[TAM_TEXTO];
} Nome;

typedef struct {
    unsigned chamadasBusca;
    unsigned comparacoes;
} MedidaBusca;

typedef struct {
    Nome nomes[N_ORDENS][N_ARTISTAS];
    Nome amostra[N_BUSCAS];
    MedidaBusca buscas[N_ARVORES][N_ORDENS][N_BUSCAS];
} Experimento;

/* Somente artistas sao cadastrados neste experimento. O ponteiro para a
   arvore de albuns e inicializado como nas questoes 1, 2 e 3. */
typedef struct album Album;

typedef struct artistaABB {
    char nome[TAM_TEXTO];
    char tipo[TAM_TEXTO];
    char estilo[TAM_TEXTO];
    int numeroAlbuns;
    Album *albuns;
    struct artistaABB *esq;
    struct artistaABB *dir;
} ArtistaABB;

typedef struct artistaAVL {
    char nome[TAM_TEXTO];
    char tipo[TAM_TEXTO];
    char estilo[TAM_TEXTO];
    int numeroAlbuns;
    Album *albuns;
    int altura;
    struct artistaAVL *esq;
    struct artistaAVL *dir;
} ArtistaAVL;

typedef struct artistaLLRB {
    char nome[TAM_TEXTO];
    char tipo[TAM_TEXTO];
    char estilo[TAM_TEXTO];
    int numeroAlbuns;
    Album *albuns;
    Cor cor;
    struct artistaLLRB *esq;
    struct artistaLLRB *dir;
} ArtistaLLRB;

/* Insercao da ABB */
static int inserirArtistaABB(ArtistaABB **raiz, const char nome[], const char tipo[],
                             const char estilo[]) {
    int comparacao;
    ArtistaABB *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof(ArtistaABB));
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
        return inserirArtistaABB(&(*raiz)->esq, nome, tipo, estilo);
    if (comparacao > 0)
        return inserirArtistaABB(&(*raiz)->dir, nome, tipo, estilo);
    return 0;
}

static const ArtistaABB *buscarArtistaABB(const ArtistaABB *raiz,
                                          const char nome[], MedidaBusca *medida) {
    int comparacao;

    medida->chamadasBusca++;
    if (raiz == NULL)
        return NULL;
    medida->comparacoes++;
    comparacao = strcmp(nome, raiz->nome);
    if (comparacao < 0)
        return buscarArtistaABB(raiz->esq, nome, medida);
    if (comparacao > 0)
        return buscarArtistaABB(raiz->dir, nome, medida);
    return raiz;
}

static void liberarABB(ArtistaABB *raiz) {
    if (raiz != NULL) {
        liberarABB(raiz->esq);
        liberarABB(raiz->dir);
        free(raiz);
    }
}

/* Insercao da AVL */
static int alturaArtistaAVL(const ArtistaAVL *no) {
    return no == NULL ? 0 : no->altura;
}

static void atualizarAlturaArtistaAVL(ArtistaAVL *no) {
    int esquerda = alturaArtistaAVL(no->esq);
    int direita = alturaArtistaAVL(no->dir);
    no->altura = 1 + (esquerda > direita ? esquerda : direita);
}

static int fatorBalanceamentoArtistaAVL(const ArtistaAVL *no) {
    return alturaArtistaAVL(no->esq) - alturaArtistaAVL(no->dir);
}

static ArtistaAVL *rotacionarEsquerdaArtistaAVL(ArtistaAVL *raiz) {
    ArtistaAVL *novaRaiz = raiz->dir;
    raiz->dir = novaRaiz->esq;
    novaRaiz->esq = raiz;
    atualizarAlturaArtistaAVL(raiz);
    atualizarAlturaArtistaAVL(novaRaiz);
    return novaRaiz;
}

static ArtistaAVL *rotacionarDireitaArtistaAVL(ArtistaAVL *raiz) {
    ArtistaAVL *novaRaiz = raiz->esq;
    raiz->esq = novaRaiz->dir;
    novaRaiz->dir = raiz;
    atualizarAlturaArtistaAVL(raiz);
    atualizarAlturaArtistaAVL(novaRaiz);
    return novaRaiz;
}

static ArtistaAVL *balancearArtistaAVL(ArtistaAVL *raiz) {
    int fator;

    atualizarAlturaArtistaAVL(raiz);
    fator = fatorBalanceamentoArtistaAVL(raiz);

    if (fator > 1) {
        if (fatorBalanceamentoArtistaAVL(raiz->esq) < 0)
            raiz->esq = rotacionarEsquerdaArtistaAVL(raiz->esq);
        return rotacionarDireitaArtistaAVL(raiz);
    }
    if (fator < -1) {
        if (fatorBalanceamentoArtistaAVL(raiz->dir) > 0)
            raiz->dir = rotacionarDireitaArtistaAVL(raiz->dir);
        return rotacionarEsquerdaArtistaAVL(raiz);
    }
    return raiz;
}

static int inserirArtistaAVL(ArtistaAVL **raiz, const char nome[], const char tipo[],
                             const char estilo[]) {
    int comparacao;
    int resultado;
    ArtistaAVL *novo;

    if (*raiz == NULL) {
        novo = malloc(sizeof(ArtistaAVL));
        if (novo == NULL)
            return -1;
        strcpy(novo->nome, nome);
        strcpy(novo->tipo, tipo);
        strcpy(novo->estilo, estilo);
        novo->numeroAlbuns = 0;
        novo->albuns = NULL;
        novo->altura = 1;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }
    comparacao = strcmp(nome, (*raiz)->nome);
    if (comparacao < 0)
        resultado = inserirArtistaAVL(&(*raiz)->esq, nome, tipo, estilo);
    else if (comparacao > 0)
        resultado = inserirArtistaAVL(&(*raiz)->dir, nome, tipo, estilo);
    else
        return 0;

    if (resultado == 1)
        *raiz = balancearArtistaAVL(*raiz);
    return resultado;
}

static const ArtistaAVL *buscarArtistaAVL(const ArtistaAVL *raiz,
                                          const char nome[], MedidaBusca *medida) {
    int comparacao;

    medida->chamadasBusca++;
    if (raiz == NULL)
        return NULL;
    medida->comparacoes++;
    comparacao = strcmp(nome, raiz->nome);
    if (comparacao < 0)
        return buscarArtistaAVL(raiz->esq, nome, medida);
    if (comparacao > 0)
        return buscarArtistaAVL(raiz->dir, nome, medida);
    return raiz;
}

static void liberarAVL(ArtistaAVL *raiz) {
    if (raiz != NULL) {
        liberarAVL(raiz->esq);
        liberarAVL(raiz->dir);
        free(raiz);
    }
}

/* Insercao da LLRB */
static Cor corOposta(Cor cor) {
    return cor == PRETO ? VERMELHO : PRETO;
}

static Cor corArtistaLLRB(const ArtistaLLRB *no) {
    return no == NULL ? PRETO : no->cor;
}

static ArtistaLLRB *rotacionarEsquerdaArtistaLLRB(ArtistaLLRB *raiz) {
    ArtistaLLRB *novaRaiz = raiz->dir;
    raiz->dir = novaRaiz->esq;
    novaRaiz->esq = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static ArtistaLLRB *rotacionarDireitaArtistaLLRB(ArtistaLLRB *raiz) {
    ArtistaLLRB *novaRaiz = raiz->esq;
    raiz->esq = novaRaiz->dir;
    novaRaiz->dir = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static void trocarCoresArtistaLLRB(ArtistaLLRB *raiz) {
    raiz->cor = corOposta(raiz->cor);
    raiz->esq->cor = corOposta(raiz->esq->cor);
    raiz->dir->cor = corOposta(raiz->dir->cor);
}

static ArtistaLLRB *balancearArtistaLLRB(ArtistaLLRB *raiz) {
    /* Cada ajuste pode criar a condicao que o proximo teste verifica. */
    if (corArtistaLLRB(raiz->dir) == VERMELHO &&
        corArtistaLLRB(raiz->esq) == PRETO)
        raiz = rotacionarEsquerdaArtistaLLRB(raiz);
    if (corArtistaLLRB(raiz->esq) == VERMELHO &&
        corArtistaLLRB(raiz->esq->esq) == VERMELHO)
        raiz = rotacionarDireitaArtistaLLRB(raiz);
    if (corArtistaLLRB(raiz->esq) == VERMELHO &&
        corArtistaLLRB(raiz->dir) == VERMELHO)
        trocarCoresArtistaLLRB(raiz);
    return raiz;
}

static int inserirArtistaRecLLRB(ArtistaLLRB **raiz, const char nome[], const char tipo[],
                                 const char estilo[]) {
    int comparacao;
    int resultado;
    ArtistaLLRB *novo;

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
        resultado = inserirArtistaRecLLRB(&(*raiz)->esq, nome, tipo, estilo);
    else if (comparacao > 0)
        resultado = inserirArtistaRecLLRB(&(*raiz)->dir, nome, tipo, estilo);
    else
        return 0;

    if (resultado == 1)
        *raiz = balancearArtistaLLRB(*raiz);
    return resultado;
}

static int inserirArtistaLLRB(ArtistaLLRB **raiz, const char nome[], const char tipo[],
                              const char estilo[]) {
    int resultado = inserirArtistaRecLLRB(raiz, nome, tipo, estilo);
    if (resultado == 1)
        (*raiz)->cor = PRETO;
    return resultado;
}

static const ArtistaLLRB *buscarArtistaLLRB(const ArtistaLLRB *raiz,
                                            const char nome[], MedidaBusca *medida) {
    int comparacao;

    medida->chamadasBusca++;
    if (raiz == NULL)
        return NULL;
    medida->comparacoes++;
    comparacao = strcmp(nome, raiz->nome);
    if (comparacao < 0)
        return buscarArtistaLLRB(raiz->esq, nome, medida);
    if (comparacao > 0)
        return buscarArtistaLLRB(raiz->dir, nome, medida);
    return raiz;
}

static void liberarLLRB(ArtistaLLRB *raiz) {
    if (raiz != NULL) {
        liberarLLRB(raiz->esq);
        liberarLLRB(raiz->dir);
        free(raiz);
    }
}

/* Sorteio sem reposicao de combinacoes de primeiro nome e sobrenome. */
static int sortearIndice(unsigned long long *estado, int limite) {
    /* Park-Miller com semente fixa para reproduzir a sequencia em outro PC. */
    *estado = (*estado * 16807ULL) % 2147483647ULL;
    return (int)((*estado - 1ULL) % (unsigned)limite);
}

static int compararNomes(const void *a, const void *b) {
    const Nome *primeiro = a;
    const Nome *segundo = b;
    return strcmp(primeiro->texto, segundo->texto);
}

static void prepararNomes(Experimento *exp) {
    static const char *const primeiros[] = {
        "Ana", "Beto", "Clara", "Davi", "Elisa", "Flora", "Gil", "Helena",
        "Iara", "Joana", "Lia", "Mauro", "Nina", "Rui", "Sara", "Zeca"
    };
    static const char *const sobrenomes[] = {
        "Alves", "Barros", "Costa", "Dias", "Freitas", "Lima", "Lopes", "Luz",
        "Matos", "Melo", "Moura", "Nunes", "Prado", "Reis", "Silva", "Sousa"
    };
    enum {
        N_PRIMEIROS = sizeof primeiros / sizeof primeiros[0],
        N_SOBRENOMES = sizeof sobrenomes / sizeof sobrenomes[0],
        COMBINACOES = N_PRIMEIROS * N_SOBRENOMES
    };
    _Static_assert(N_ARTISTAS <= COMBINACOES, "Sao necessarios mais nomes distintos");
    int codigos[COMBINACOES];
    unsigned long long estado = SEMENTE;

    for (int i = 0; i < COMBINACOES; i++)
        codigos[i] = i;
    for (int i = 0; i < N_ARTISTAS; i++) {
        int posicao = i + sortearIndice(&estado, COMBINACOES - i);
        int codigo = codigos[posicao];
        codigos[posicao] = codigos[i];
        codigos[i] = codigo;
        snprintf(exp->nomes[ALEATORIA][i].texto, TAM_TEXTO, "%s %s",
                 primeiros[codigo / N_SOBRENOMES], sobrenomes[codigo % N_SOBRENOMES]);
    }

    /* A ordem de sorteio ja e a ordem aleatoria das tres arvores. */
    memcpy(exp->nomes[CRESCENTE], exp->nomes[ALEATORIA], sizeof exp->nomes[CRESCENTE]);
    qsort(exp->nomes[CRESCENTE], N_ARTISTAS, sizeof(Nome), compararNomes);
    for (int i = 0; i < N_ARTISTAS; i++)
        exp->nomes[DECRESCENTE][i] = exp->nomes[CRESCENTE][N_ARTISTAS - 1 - i];
}

/* Primeiro monta a arvore. Cada uma das dez buscas recebe contadores zerados. */
static int medirCenario(Experimento *exp, TipoArvore arvore, Ordem ordem) {
    ArtistaABB *abb = NULL;
    ArtistaAVL *avl = NULL;
    ArtistaLLRB *llrb = NULL;
    int sucesso = 0;

    for (int i = 0; i < N_ARTISTAS; i++) {
        const char *nome = exp->nomes[ordem][i].texto;
        int resultado;
        if (arvore == ABB)
            resultado = inserirArtistaABB(&abb, nome, "Cantor", "MPB");
        else if (arvore == AVL)
            resultado = inserirArtistaAVL(&avl, nome, "Cantor", "MPB");
        else
            resultado = inserirArtistaLLRB(&llrb, nome, "Cantor", "MPB");

        if (resultado != 1) {
            fprintf(stderr, "Falha ao inserir %s em %s/%s: %s.\n",
                    nome, nomeArvore[arvore], nomeOrdem[ordem],
                    resultado == 0 ? "nome repetido" : "memoria insuficiente");
            goto liberar;
        }
    }

    for (int i = 0; i < N_BUSCAS; i++) {
        const char *nome = exp->amostra[i].texto;
        MedidaBusca medida = {0};
        int encontrado;

        if (arvore == ABB)
            encontrado = buscarArtistaABB(abb, nome, &medida) != NULL;
        else if (arvore == AVL)
            encontrado = buscarArtistaAVL(avl, nome, &medida) != NULL;
        else
            encontrado = buscarArtistaLLRB(llrb, nome, &medida) != NULL;

        if (!encontrado) {
            fprintf(stderr, "Artista ausente em %s/%s: %s.\n",
                    nomeArvore[arvore], nomeOrdem[ordem], nome);
            goto liberar;
        }
        exp->buscas[arvore][ordem][i] = medida;
    }
    sucesso = 1;

liberar:
    liberarABB(abb);
    liberarAVL(avl);
    liberarLLRB(llrb);
    return sucesso;
}

static unsigned passos(const MedidaBusca *medida) {
    return medida->chamadasBusca + medida->comparacoes;
}

static int salvarCSV(const Experimento *exp) {
    /* Publica o arquivo completo somente depois de escrever as 90 linhas. */
    char temporario[80];
#ifdef _WIN32
    unsigned long processo = (unsigned long)GetCurrentProcessId();
#else
    unsigned long processo = (unsigned long)getpid();
#endif
    snprintf(temporario, sizeof temporario, "questao4b_%lu.tmp", processo);
    FILE *saida = fopen(temporario, "w");
    if (saida == NULL)
        return 0;

    fputs("arvore;ordem;posicao_no_sorteio;artista;chamadas_buscarArtista;chamadas_strcmp;passos\n",
          saida);
    for (int a = 0; a < N_ARVORES; a++) {
        for (int o = 0; o < N_ORDENS; o++) {
            for (int i = 0; i < N_BUSCAS; i++) {
                const MedidaBusca *m = &exp->buscas[a][o][i];
                fprintf(saida, "%s;%s;%d;%s;%u;%u;%u\n", nomeArvore[a],
                        nomeOrdem[o], posicoesSorteio[i], exp->amostra[i].texto,
                        m->chamadasBusca, m->comparacoes, passos(m));
            }
        }
    }

    int completo = !ferror(saida);
    if (fclose(saida) != 0)
        completo = 0;
    if (completo) {
#ifdef _WIN32
        completo = MoveFileExA(temporario, ARQUIVO_CSV, MOVEFILE_REPLACE_EXISTING) != 0;
#else
        completo = rename(temporario, ARQUIVO_CSV) == 0;
#endif
    }
    if (!completo)
        remove(temporario);
    return completo;
}

static void mostrarResumo(const Experimento *exp) {
    double medias[N_ARVORES][N_ORDENS];
    puts("Questao 4(b) - busca de artistas");
    printf("%d cenarios | %d nomes em cada arvore pronta | %d buscas | semente %u\n",
           N_ARVORES * N_ORDENS, N_ARTISTAS, N_ARVORES * N_ORDENS * N_BUSCAS, SEMENTE);
    puts("Passos = chamadas de buscarArtista + chamadas de strcmp durante a busca.");
    puts("Cada comparacao corresponde a um no visitado.\n");
    printf("%-5s %-13s %9s %9s %11s %11s %12s\n", "Tipo", "Insercao",
           "MinPassos", "MaxPassos", "SomaPassos", "MediaPassos", "MediaNos");

    for (int a = 0; a < N_ARVORES; a++) {
        for (int o = 0; o < N_ORDENS; o++) {
            const MedidaBusca *medidas = exp->buscas[a][o];
            unsigned minimo = passos(&medidas[0]), maximo = minimo;
            unsigned soma = 0, nos = 0;
            for (int i = 0; i < N_BUSCAS; i++) {
                unsigned p = passos(&medidas[i]);
                if (p < minimo)
                    minimo = p;
                if (p > maximo)
                    maximo = p;
                soma += p;
                nos += medidas[i].comparacoes;
            }
            medias[a][o] = (double)soma / N_BUSCAS;
            printf("%-5s %-13s %9u %9u %11u %11.1f %12.1f\n",
                   nomeArvore[a], nomeOrdem[o], minimo, maximo, soma,
                   medias[a][o], (double)nos / N_BUSCAS);
        }
    }

    printf("\nNa ABB aleatoria, %s ficou na raiz e exigiu %u passos. "
           "A busca por %s exigiu %u passos.\n",
           exp->amostra[0].texto, passos(&exp->buscas[ABB][ALEATORIA][0]),
           exp->amostra[1].texto, passos(&exp->buscas[ABB][ALEATORIA][1]));
    printf("Na ABB, a media foi %.1f na ordem crescente, %.1f na decrescente "
           "e %.1f na aleatoria. Nas entradas ordenadas os artistas formam "
           "uma cadeia.\n", medias[ABB][CRESCENTE], medias[ABB][DECRESCENTE],
           medias[ABB][ALEATORIA]);
    printf("Na ordem crescente, a AVL exigiu %.1f passos por busca e a LLRB, %.1f. "
           "O balanceamento durante as insercoes reduziu os caminhos ate esses artistas.\n",
           medias[AVL][CRESCENTE], medias[LLRB][CRESCENTE]);
}

int main(void) {
    Experimento exp;
    prepararNomes(&exp);
    for (int i = 0; i < N_BUSCAS; i++)
        exp.amostra[i] = exp.nomes[ALEATORIA][posicoesSorteio[i] - 1];

    for (int a = 0; a < N_ARVORES; a++) {
        for (int o = 0; o < N_ORDENS; o++) {
            if (!medirCenario(&exp, (TipoArvore)a, (Ordem)o))
                return EXIT_FAILURE;
        }
    }
    if (!salvarCSV(&exp)) {
        fprintf(stderr, "Nao foi possivel gravar %s na pasta atual.\n", ARQUIVO_CSV);
        return EXIT_FAILURE;
    }

    mostrarResumo(&exp);
    puts("\nResultados individuais: " ARQUIVO_CSV);
    return EXIT_SUCCESS;
}