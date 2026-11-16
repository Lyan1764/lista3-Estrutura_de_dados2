/*
 * Estruturas de Dados II - Questao 4(a): insercao de artistas.
 * Compilar: gcc -std=c11 -O2 -Wall -Wextra -pedantic questao4a_insercao_artistas.c -o questao4a
 *
 * Cada linha de questao4a_insercoes.csv corresponde a uma insercao.
 * Um passo e uma chamada no codigo-fonte da insercao, incluindo cada entrada
 * recursiva, as rotacoes, strcmp, malloc e strcpy. Os tempos sao medidos com
 * os contadores ativos. Geracao dos nomes, ordenacao e escrita ficam fora.
 */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <unistd.h>
#endif

#define N_ARTISTAS 100
#define TAM_TEXTO 100
#define SEMENTE 2026U
#define ARQUIVO_CSV "questao4a_insercoes.csv"

typedef enum { ABB, AVL, LLRB, N_ARVORES } TipoArvore;
typedef enum { CRESCENTE, DECRESCENTE, ALEATORIA, N_ORDENS } Ordem;
typedef enum { PRETO, VERMELHO } Cor;

static const char *const nomeArvore[N_ARVORES] = {"ABB", "AVL", "LLRB"};
static const char *const nomeOrdem[N_ORDENS] = {"crescente", "decrescente", "aleatoria"};

/* Os nomes das colunas sao os das funcoes nos tres programas da lista. */
enum Funcao {
    INSERIR, INSERIR_REC, COMPARAR, ALOCAR, COPIAR, ALTURA, ATUALIZAR_ALTURA,
    FATOR, BALANCEAR, ROT_ESQ, ROT_DIR, COR, COR_OPOSTA, TROCAR_CORES, N_FUNCOES
};

static const char *const nomeFuncao[N_FUNCOES] = {
    "inserirArtista", "inserirArtistaRec", "strcmp", "malloc", "strcpy",
    "alturaArtista", "atualizarAlturaArtista", "fatorBalanceamentoArtista",
    "balancearArtista", "rotacionarEsquerdaArtista", "rotacionarDireitaArtista",
    "corArtista", "corOposta", "trocarCoresArtista"
};

static unsigned chamadas[N_FUNCOES];

typedef struct {
    char texto[TAM_TEXTO];
} Nome;

typedef struct {
    long long tempoNs;
    unsigned chamadas[N_FUNCOES];
} Medida;

typedef struct {
    Nome nomes[N_ORDENS][N_ARTISTAS];
    Medida insercoes[N_ARVORES][N_ORDENS][N_ARTISTAS];
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

    chamadas[INSERIR]++;
    if (*raiz == NULL) {
        chamadas[ALOCAR]++;
        novo = malloc(sizeof(ArtistaABB));
        if (novo == NULL)
            return -1;
        chamadas[COPIAR]++;
        strcpy(novo->nome, nome);
        chamadas[COPIAR]++;
        strcpy(novo->tipo, tipo);
        chamadas[COPIAR]++;
        strcpy(novo->estilo, estilo);
        novo->numeroAlbuns = 0;
        novo->albuns = NULL;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }
    chamadas[COMPARAR]++;
    comparacao = strcmp(nome, (*raiz)->nome);
    if (comparacao < 0)
        return inserirArtistaABB(&(*raiz)->esq, nome, tipo, estilo);
    if (comparacao > 0)
        return inserirArtistaABB(&(*raiz)->dir, nome, tipo, estilo);
    return 0;
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
    chamadas[ALTURA]++;
    return no == NULL ? 0 : no->altura;
}

static void atualizarAlturaArtistaAVL(ArtistaAVL *no) {
    chamadas[ATUALIZAR_ALTURA]++;
    int esquerda = alturaArtistaAVL(no->esq);
    int direita = alturaArtistaAVL(no->dir);
    no->altura = 1 + (esquerda > direita ? esquerda : direita);
}

static int fatorBalanceamentoArtistaAVL(const ArtistaAVL *no) {
    chamadas[FATOR]++;
    return alturaArtistaAVL(no->esq) - alturaArtistaAVL(no->dir);
}

static ArtistaAVL *rotacionarEsquerdaArtistaAVL(ArtistaAVL *raiz) {
    chamadas[ROT_ESQ]++;
    ArtistaAVL *novaRaiz = raiz->dir;
    raiz->dir = novaRaiz->esq;
    novaRaiz->esq = raiz;
    atualizarAlturaArtistaAVL(raiz);
    atualizarAlturaArtistaAVL(novaRaiz);
    return novaRaiz;
}

static ArtistaAVL *rotacionarDireitaArtistaAVL(ArtistaAVL *raiz) {
    chamadas[ROT_DIR]++;
    ArtistaAVL *novaRaiz = raiz->esq;
    raiz->esq = novaRaiz->dir;
    novaRaiz->dir = raiz;
    atualizarAlturaArtistaAVL(raiz);
    atualizarAlturaArtistaAVL(novaRaiz);
    return novaRaiz;
}

static ArtistaAVL *balancearArtistaAVL(ArtistaAVL *raiz) {
    int fator;
    chamadas[BALANCEAR]++;
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

    chamadas[INSERIR]++;
    if (*raiz == NULL) {
        chamadas[ALOCAR]++;
        novo = malloc(sizeof(ArtistaAVL));
        if (novo == NULL)
            return -1;
        chamadas[COPIAR]++;
        strcpy(novo->nome, nome);
        chamadas[COPIAR]++;
        strcpy(novo->tipo, tipo);
        chamadas[COPIAR]++;
        strcpy(novo->estilo, estilo);
        novo->numeroAlbuns = 0;
        novo->albuns = NULL;
        novo->altura = 1;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }
    chamadas[COMPARAR]++;
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

static void liberarAVL(ArtistaAVL *raiz) {
    if (raiz != NULL) {
        liberarAVL(raiz->esq);
        liberarAVL(raiz->dir);
        free(raiz);
    }
}

/* Insercao da LLRB */
static Cor corOposta(Cor cor) {
    chamadas[COR_OPOSTA]++;
    return cor == PRETO ? VERMELHO : PRETO;
}

static Cor corArtistaLLRB(const ArtistaLLRB *no) {
    chamadas[COR]++;
    return no == NULL ? PRETO : no->cor;
}

static ArtistaLLRB *rotacionarEsquerdaArtistaLLRB(ArtistaLLRB *raiz) {
    chamadas[ROT_ESQ]++;
    ArtistaLLRB *novaRaiz = raiz->dir;
    raiz->dir = novaRaiz->esq;
    novaRaiz->esq = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static ArtistaLLRB *rotacionarDireitaArtistaLLRB(ArtistaLLRB *raiz) {
    chamadas[ROT_DIR]++;
    ArtistaLLRB *novaRaiz = raiz->esq;
    raiz->esq = novaRaiz->dir;
    novaRaiz->dir = raiz;
    novaRaiz->cor = raiz->cor;
    raiz->cor = VERMELHO;
    return novaRaiz;
}

static void trocarCoresArtistaLLRB(ArtistaLLRB *raiz) {
    chamadas[TROCAR_CORES]++;
    raiz->cor = corOposta(raiz->cor);
    raiz->esq->cor = corOposta(raiz->esq->cor);
    raiz->dir->cor = corOposta(raiz->dir->cor);
}

static ArtistaLLRB *balancearArtistaLLRB(ArtistaLLRB *raiz) {
    chamadas[BALANCEAR]++;
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

    chamadas[INSERIR_REC]++;
    if (*raiz == NULL) {
        chamadas[ALOCAR]++;
        novo = malloc(sizeof *novo);
        if (novo == NULL)
            return -1;
        chamadas[COPIAR]++;
        strcpy(novo->nome, nome);
        chamadas[COPIAR]++;
        strcpy(novo->tipo, tipo);
        chamadas[COPIAR]++;
        strcpy(novo->estilo, estilo);
        novo->numeroAlbuns = 0;
        novo->albuns = NULL;
        novo->cor = VERMELHO;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }

    chamadas[COMPARAR]++;
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
    chamadas[INSERIR]++;
    int resultado = inserirArtistaRecLLRB(raiz, nome, tipo, estilo);
    if (resultado == 1)
        (*raiz)->cor = PRETO;
    return resultado;
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

/* Frequencia em tiques por segundo. No POSIX usamos diretamente nanossegundos. */
static long long frequenciaRelogio(void) {
#ifdef _WIN32
    LARGE_INTEGER frequencia;
    if (!QueryPerformanceFrequency(&frequencia))
        return 0;
    return frequencia.QuadPart;
#else
    return 1000000000LL;
#endif
}

static long long lerRelogio(void) {
#ifdef _WIN32
    LARGE_INTEGER instante;
    if (!QueryPerformanceCounter(&instante))
        return -1;
    return instante.QuadPart;
#else
    struct timespec instante;
    if (clock_gettime(CLOCK_MONOTONIC, &instante) != 0)
        return -1;
    return (long long)instante.tv_sec * 1000000000LL + instante.tv_nsec;
#endif
}

static int medirCenario(TipoArvore arvore, const Nome vetor[], Medida medidas[],
                        double nsPorTique) {
    ArtistaABB *abb = NULL;
    ArtistaAVL *avl = NULL;
    ArtistaLLRB *llrb = NULL;
    int i;

    for (i = 0; i < N_ARTISTAS; i++) {
        const char *nome = vetor[i].texto;
        int resultado;
        long long inicio, fim;
        memset(chamadas, 0, sizeof chamadas);

        if (arvore == ABB) {
            inicio = lerRelogio();
            resultado = inserirArtistaABB(&abb, nome, "Cantor", "MPB");
        } else if (arvore == AVL) {
            inicio = lerRelogio();
            resultado = inserirArtistaAVL(&avl, nome, "Cantor", "MPB");
        } else {
            inicio = lerRelogio();
            resultado = inserirArtistaLLRB(&llrb, nome, "Cantor", "MPB");
        }
        fim = lerRelogio();

        if (resultado != 1) {
            fprintf(stderr, "Falha ao inserir %s: %s.\n", nome,
                    resultado == 0 ? "nome repetido" : "memoria insuficiente");
            break;
        }
        if (inicio < 0 || fim < inicio) {
            fprintf(stderr, "Falha na leitura do relogio.\n");
            break;
        }
        medidas[i].tempoNs = (long long)((double)(fim - inicio) * nsPorTique + 0.5);
        memcpy(medidas[i].chamadas, chamadas, sizeof chamadas);
    }

    liberarABB(abb);
    liberarAVL(avl);
    liberarLLRB(llrb);
    return i == N_ARTISTAS;
}

static unsigned totalPassos(const Medida *medida) {
    unsigned total = 0;
    for (int f = 0; f < N_FUNCOES; f++)
        total += medida->chamadas[f];
    return total;
}

static int salvarCSV(const Experimento *exp) {
    /* Publica o arquivo completo somente depois de escrever as 900 linhas. */
    char temporario[80];
#ifdef _WIN32
    unsigned long processo = (unsigned long)GetCurrentProcessId();
#else
    unsigned long processo = (unsigned long)getpid();
#endif
    snprintf(temporario, sizeof temporario, "questao4a_%lu.tmp", processo);
    FILE *saida = fopen(temporario, "w");
    if (saida == NULL)
        return 0;

    fputs("arvore;ordem;posicao;artista;tempo_ns;passos", saida);
    for (int f = 0; f < N_FUNCOES; f++)
        fprintf(saida, ";%s", nomeFuncao[f]);
    fputc('\n', saida);

    for (int a = 0; a < N_ARVORES; a++) {
        for (int o = 0; o < N_ORDENS; o++) {
            for (int i = 0; i < N_ARTISTAS; i++) {
                const Medida *m = &exp->insercoes[a][o][i];
                fprintf(saida, "%s;%s;%d;%s;%lld;%u", nomeArvore[a], nomeOrdem[o],
                        i + 1, exp->nomes[o][i].texto, m->tempoNs, totalPassos(m));
                for (int f = 0; f < N_FUNCOES; f++)
                    fprintf(saida, ";%u", m->chamadas[f]);
                fputc('\n', saida);
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
    Medida totais[N_ARVORES][N_ORDENS] = {0};
    puts("\nQuestao 4(a) - insercao de artistas");
    printf("%d artistas distintos | semente %u | %d insercoes medidas\n",
           N_ARTISTAS, SEMENTE, N_ARVORES * N_ORDENS * N_ARTISTAS);
    puts("Passos = soma das chamadas. Rotacoes e comparacoes ja entram nessa soma.");
    puts("1a e ultima = passos nessas duas insercoes. Media(ns) = tempo medio por artista.\n");
    printf("%-5s %-12s %6s %6s %9s %8s %7s %7s %11s\n", "Tipo", "Ordem",
           "1a", "Ultima", "Passos", "strcmp", "Rot.E", "Rot.D", "Media(ns)");

    for (int a = 0; a < N_ARVORES; a++) {
        for (int o = 0; o < N_ORDENS; o++) {
            Medida *total = &totais[a][o];
            const Medida *linha = exp->insercoes[a][o];
            for (int i = 0; i < N_ARTISTAS; i++) {
                total->tempoNs += linha[i].tempoNs;
                for (int f = 0; f < N_FUNCOES; f++)
                    total->chamadas[f] += linha[i].chamadas[f];
            }
            printf("%-5s %-12s %6u %6u %9u %8u %7u %7u %11.1f\n",
                   nomeArvore[a], nomeOrdem[o], totalPassos(&linha[0]),
                   totalPassos(&linha[N_ARTISTAS - 1]), totalPassos(total),
                   total->chamadas[COMPARAR], total->chamadas[ROT_ESQ],
                   total->chamadas[ROT_DIR], (double)total->tempoNs / N_ARTISTAS);
        }
    }

    const Medida *crescente = exp->insercoes[ABB][CRESCENTE];
    printf("\nNa ABB crescente, inserirArtista foi chamada %u vez na primeira "
           "insercao e %u vezes na ultima (%u chamadas nas %d insercoes).\n",
           crescente[0].chamadas[INSERIR], crescente[N_ARTISTAS - 1].chamadas[INSERIR],
           totais[ABB][CRESCENTE].chamadas[INSERIR], N_ARTISTAS);
    printf("A ABB fez %u comparacoes na ordem crescente, %u na decrescente e %u na "
           "aleatoria. Nas duas ordens classificadas, cada novo nome prolongou "
           "um unico ramo da arvore.\n", totais[ABB][CRESCENTE].chamadas[COMPARAR],
           totais[ABB][DECRESCENTE].chamadas[COMPARAR], totais[ABB][ALEATORIA].chamadas[COMPARAR]);
    printf("Ao inverter a ordem crescente, os passos da AVL passaram de %u para %u, "
           "e os da LLRB, de %u para %u. Na AVL as rotacoes sao simetricas. "
           "Na LLRB o balanceamento orienta os elos vermelhos para a esquerda.\n",
           totalPassos(&totais[AVL][CRESCENTE]), totalPassos(&totais[AVL][DECRESCENTE]),
           totalPassos(&totais[LLRB][CRESCENTE]), totalPassos(&totais[LLRB][DECRESCENTE]));
    printf("Com a mesma entrada crescente, a AVL fez %u comparacoes e a LLRB fez %u. "
           "As colunas Rot.E e Rot.D mostram o custo em rotacoes para manter "
           "as arvores balanceadas.\n", totais[AVL][CRESCENTE].chamadas[COMPARAR],
           totais[LLRB][CRESCENTE].chamadas[COMPARAR]);
    puts("Cada tempo e uma observacao da versao instrumentada e depende da maquina.");
}

int main(void) {
    static Experimento exp;
    long long frequencia = frequenciaRelogio();
    if (frequencia <= 0) {
        fputs("Relogio de alta resolucao indisponivel.\n", stderr);
        return EXIT_FAILURE;
    }
    double nsPorTique = 1000000000.0 / (double)frequencia;
    prepararNomes(&exp);
    for (int a = 0; a < N_ARVORES; a++) {
        for (int o = 0; o < N_ORDENS; o++) {
            if (!medirCenario((TipoArvore)a, exp.nomes[o], exp.insercoes[a][o], nsPorTique)) {
                fprintf(stderr, "Cenario interrompido: %s / %s.\n", nomeArvore[a], nomeOrdem[o]);
                return EXIT_FAILURE;
            }
        }
    }
    if (!salvarCSV(&exp)) {
        fprintf(stderr, "Nao foi possivel gravar %s na pasta atual.\n", ARQUIVO_CSV);
        return EXIT_FAILURE;
    }
    mostrarResumo(&exp);
    puts("\nDados individuais: " ARQUIVO_CSV);
    return EXIT_SUCCESS;
}
