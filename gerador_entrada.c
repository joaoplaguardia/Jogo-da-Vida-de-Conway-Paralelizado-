#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Uso: %s <n_gen> <L> <C> <n_cel>\n", argv[0]);
        return 1;
    }

    int n_gen = atoi(argv[1]);
    int L = atoi(argv[2]);
    int C = atoi(argv[3]);
    int n_cel = atoi(argv[4]);

    FILE *f = fopen("entrada.txt", "w");
    if (!f) {
        perror("Erro ao criar arquivo");
        return 1;
    }

    fprintf(f, "%d %d %d %d\n", n_gen, L, C, n_cel);

    srand(time(NULL));

    for (int i = 0; i < n_cel; i++) {
        int x = rand() % L;
        int y = rand() % C;
        fprintf(f, "%d %d\n", x, y);
    }

    fclose(f);

    printf("Arquivo entrada.txt gerado com sucesso!\n");
    return 0;
}
