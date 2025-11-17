#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

pthread_barrier_t barrier;

typedef struct {
    int start_row;
    int end_row;
    int **matriz_atual;
    int **matriz_prox;
    int n_gen;
    int L;
    int C;
} ThreadRegion;

int vizinhas(int **matriz, int i, int j, int L, int C) {
    int cont = 0;
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            if(x == 0 && y == 0) continue;
            int novo_i = i + x;
            int novo_j = j + y;
            if(novo_i >= 0 && novo_i < L && novo_j >= 0 && novo_j < C) {
                cont += matriz[novo_i][novo_j];
            }
        }
    }
    return cont;
}

void* jogo_da_vida(void *region_prt) {
    ThreadRegion *region = (ThreadRegion*) region_prt;
    int start_row = region->start_row;
    int end_row = region->end_row;
    int n_gen = region->n_gen;
    int L = region->L;
    int C = region->C;

    for(int k = 0; k < n_gen; k++) {

        int **matriz_atual = region->matriz_atual;
        int **matriz_prox = region->matriz_prox;

        for(int i = start_row; i < end_row; i++) {
            for(int j = 0; j < C; j++) {
                int viz = vizinhas(matriz_atual, i, j, L, C);

                if(matriz_atual[i][j] == 1) {
                    matriz_prox[i][j] = (viz == 2 || viz == 3);
                } else {
                    matriz_prox[i][j] = (viz == 3);
                }
            }
        }

        pthread_barrier_wait(&barrier);  // sinaliza término da geração
        pthread_barrier_wait(&barrier);  // espera main trocar buffers
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <arquivo_entrada> <num_threads>\n", argv[0]);
        return 1;
    }

    char *arquivo_nome = argv[1];
    int NUM_THREADS = atoi(argv[2]);

    if (NUM_THREADS < 1) {
        printf("Número de threads inválido.\n");
        return 1;
    }

    FILE *arquivo = fopen(arquivo_nome, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    int n_gen, L, C, n_cel;
    fscanf(arquivo, "%d %d %d %d", &n_gen, &L, &C, &n_cel);

    if (NUM_THREADS > L) NUM_THREADS = L; // não cria threads inúteis

    int **coordenadas = malloc(n_cel * sizeof(int*));
    for (int i = 0; i < n_cel; i++) {
        coordenadas[i] = malloc(2 * sizeof(int));
        fscanf(arquivo, "%d %d", &coordenadas[i][0], &coordenadas[i][1]);
    }
    fclose(arquivo);

    int **matriz_atual = malloc(L * sizeof(int*));
    int **matriz_prox  = malloc(L * sizeof(int*));
    for (int i = 0; i < L; i++) {
        matriz_atual[i] = calloc(C, sizeof(int));
        matriz_prox[i]  = calloc(C, sizeof(int));
    }

    for (int i = 0; i < n_cel; i++) {
        int x = coordenadas[i][0];
        int y = coordenadas[i][1];
        matriz_atual[x][y] = 1;
        free(coordenadas[i]);
    }
    free(coordenadas);

    pthread_t *threads = malloc(NUM_THREADS * sizeof(pthread_t));
    ThreadRegion *regions = malloc(NUM_THREADS * sizeof(ThreadRegion));

    pthread_barrier_init(&barrier, NULL, NUM_THREADS + 1);

    int rows_per_thread = L / NUM_THREADS;
    int resto = L % NUM_THREADS;

    int linha = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        regions[i].start_row = linha;
        int add = (i < resto) ? 1 : 0;
        linha += rows_per_thread + add;
        regions[i].end_row = linha;

        regions[i].matriz_atual = matriz_atual;
        regions[i].matriz_prox  = matriz_prox;
        regions[i].n_gen = n_gen;
        regions[i].L = L;
        regions[i].C = C;
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, jogo_da_vida, &regions[i]);
    }

    // loop principal
    for (int gen = 0; gen < n_gen; gen++) {
        pthread_barrier_wait(&barrier);

        int **tmp = matriz_atual;
        matriz_atual = matriz_prox;
        matriz_prox  = tmp;

        for (int t = 0; t < NUM_THREADS; t++) {
            regions[t].matriz_atual = matriz_atual;
            regions[t].matriz_prox  = matriz_prox;
        }

        pthread_barrier_wait(&barrier);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // contar vivas
    int contagem = 0;
    for(int i = 0; i < L; i++)
        for(int j = 0; j < C; j++)
            if(matriz_atual[i][j] == 1)
                contagem++;

    FILE *saida = fopen("saida.txt", "w");
    fprintf(saida, "%d\n", contagem);
    for(int i = 0; i < L; i++)
        for(int j = 0; j < C; j++)
            if(matriz_atual[i][j] == 1)
                fprintf(saida, "%d %d\n", i, j);

    fclose(saida);

    for (int i = 0; i < L; i++) {
        free(matriz_atual[i]);
        free(matriz_prox[i]);
    }
    free(matriz_atual);
    free(matriz_prox);
    free(threads);
    free(regions);

    pthread_barrier_destroy(&barrier);

    return 0;
}
