#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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

void main() {
    FILE *arquivo = fopen("entrada.txt", "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
    }

    int n_gen, L, C, n_cel;
    fscanf(arquivo, "%d %d %d %d", &n_gen, &L, &C, &n_cel);


    int **coordenadas = malloc(n_cel * sizeof(int*));
    for (int i = 0; i < n_cel; i++) {
        coordenadas[i] = malloc(2 * sizeof(int)); // duas colunas (x, y)
    }

    for(int i = 0; i < n_cel; i++) {
        fscanf(arquivo, "%d %d", &coordenadas[i][0], &coordenadas[i][1]);
    }

    fclose(arquivo);

    int **matriz_atual = malloc(L * sizeof(int*)); 
    int **matriz_prox = malloc(L * sizeof(int*));
    for (int i = 0; i < L; i++) {
        matriz_atual[i] = malloc(C * sizeof(int));
        matriz_prox[i] = malloc(C * sizeof(int));
    }

    for (int i = 0; i < L; i++) {
        for (int j = 0; j < C; j++) {
            matriz_atual[i][j] = 0;
        }
    }

    for (int i = 0; i < n_cel; i++) {
        int x = coordenadas[i][0];
        int y = coordenadas[i][1];
        matriz_atual[x][y] = 1;
    }

    for (int i = 0; i < n_cel; i++) {
        free(coordenadas[i]);
    }
    free(coordenadas);


    for(int k = 0; k < n_gen; k++) {

        for(int i = 0; i < L; i++) {
            for( int j = 0; j < C; j++) {
                int viz = vizinhas(matriz_atual, i, j, L, C);
                
                if(matriz_atual[i][j] == 1) {
                    if(viz < 2) {
                        matriz_prox[i][j] = 0;
                    } else if(viz > 3) {
                        matriz_prox[i][j] = 0;
                    } else {
                        matriz_prox[i][j] = 1;
                    }
                } else {
                    if(viz == 3) {
                        matriz_prox[i][j] = 1;
                    } else {
                        matriz_prox[i][j] = 0;
                    }
                }
            }
        }

        
        int **temp = matriz_atual;
        matriz_atual = matriz_prox;
        matriz_prox = temp;
    }

    int contagem = 0;
    for(int i = 0; i < L; i++) {
        for(int j = 0; j < C; j++) {
            if(matriz_atual[i][j] == 1) {
                contagem++;
            }
        }
    }

    FILE *saida = fopen("saida.txt", "w");
    if (saida == NULL) {
        perror("Erro ao abrir o arquivo de saída");
    }

    fprintf(saida, "%d\n", contagem);

    for(int i = 0; i < L; i++) {
        for(int j = 0; j < C; j++) {
            if(matriz_atual[i][j] == 1) {
                fprintf(saida, "%d %d\n", i, j);
            }
        }
    }


    for (int i = 0; i < L; i++) {
        free(matriz_atual[i]);
        free(matriz_prox[i]);
    }
    free(matriz_atual);
    free(matriz_prox);
}
