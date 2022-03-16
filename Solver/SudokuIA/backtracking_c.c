#include <stdio.h>
#include <stdlib.h>

// testa se o valor i pode ser colocado na posiçao pos do jogo em relação a linha. retorna 1 se for valido
int linha(int* estado, int i, int pos) {
    int line = pos/9;

    if (line == 0) {
        for (int j = 0; j < 9; j++)
            if (i == estado[j]) return 0;

        return 1;
    }

    if (line == 1) {
        for (int j = 9; j < 18; j++)
            if (i == estado[j]) return 0;

        return 1;
    }

    if (line == 2) {
        for (int j = 18; j < 27; j++)
            if (i == estado[j]) return 0;

        return 1;
    }

    if (line == 3) {
        for (int j = 27; j < 36; j++)
            if (i == estado[j]) return 0;

        return 1;
    }

    if (line == 4) {
        for (int j = 36; j < 45; j++)
            if (i == estado[j]) return 0;

        return 1;
    }

    if (line == 5) {
        for (int j = 45; j < 54; j++)
            if (i == estado[j]) return 0;

        return 1;
    }

    if (line == 6) {
        for (int j = 54; j < 63; j++)
            if (i == estado[j]) return 0;

        return 1;
    }

    if (line == 7) {
        for (int j = 63; j < 72; j++)
            if (i == estado[j]) return 0;

        return 1;
    }

    if (line == 8) {
        for (int j = 72; j < 81; j++)
            if (i == estado[j]) return 0;

        return 1;
    }
}

// testa se o valor i pode ser colocado na posiçao pos do jogo em relação a coluna. retorna 1 se for valido
int coluna(int* estado, int i, int pos) {
    int colun = pos%9;

    for (int j = colun; j < 81; j = j + 9)
        if (i == estado[j]) return 0;

        return 1;
}

// verifica se o quadrante não possui o numero i. retorna 1 se nao possuir
int quad(int* estado, int i, int pos) {
    int line = pos/9, colun = pos%9;
    int aux;

    if (line <= 2) {
        if (colun <= 2) {
            aux = 0;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++)
                    if (i == estado[aux + k]) return 0;
                aux = aux + 9;
            }
            return 1;
        }
        else if (colun <= 5) {
            aux = 3;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++)
                    if (i == estado[aux + k]) return 0;
                aux = aux + 9;
            }
            return 1;
        }
        else {
            aux = 6;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++)
                    if (i == estado[aux + k]) return 0;
                aux = aux + 9;
            }
            return 1;
        }
    }

    else if (line <= 5) {
        if (colun <= 2) {
            aux = 27;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++)
                    if (i == estado[aux + k]) return 0;
                aux = aux + 9;
            }
            return 1;
        }

        else if (colun <= 5) {
            aux = 30;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++)
                    if (i == estado[aux + k]) return 0;
                aux = aux + 9;
            }
            return 1;
        }
        else {
            aux = 33;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++)
                    if (i == estado[aux + k]) return 0;
                aux = aux + 9;
            }
            return 1;
        }
    }

    else {
        if (colun <= 2) {
            aux = 54;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++)
                    if (i == estado[aux + k]) return 0;
                aux = aux + 9;
            }
            return 1;
        }
        else if (colun <= 5) {
            aux = 57;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++)
                    if (i == estado[aux + k]) return 0;
                aux = aux + 9;
            }
            return 1;
        }
        else {
            aux = 60;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++)
                    if (i == estado[aux + k]) return 0;
                aux = aux + 9;
            }
            return 1;
        }

    }

    return 0;
}

//transforma uma linha do arquivo em um vetor de int e guarda em um no
void transform(char* line, int* estado) {
    for (int i = 0; i < 81; i++) {
        if (line[i] == '.')
            estado[i] = 0;
        else 
            estado[i] = line[i] - 48;
    }
}

//verifica se o jogo terminou ou se ainda tem lugares a ser preenchidos
int verifica(int* estado) {
    for (int i = 0; i < 81; i++)
        if (estado[i] == 0) return 0;
    return 1;
}

//verifica se o numero i pode ser colocado na pos i. retorna 1 se for verdade
int valido(int *estado, int i, int pos) {
    if (linha(estado, i, pos))
        if (coluna(estado, i, pos))
            if (quad(estado, i, pos))
                return 1;
    return 0;
}

int heu(int* estado) {  //heuristica que retorna a posição que tem menos possibilidades de gerar uma arvore grande
    for (int i = 1; i <= 9; i++) { 
        for (int j = 0; j < 81; j++) {
            int count = 0;
            if (estado[j] == 0) {
                for (int k = 1; k <= 9; k++)
                    if (valido(estado, k, j)) count++;
            }
            if (count == i) return j;
        }
    }
    return -1; //se não encontrar posição com possibilidade retorna -1
}

//imprime o vetor guardado no nó estado
void imprimir(int* estado) {
    for (int i = 0; i < 81; i++)
        printf("%d", estado[i]);
    printf("\n");
}

int Revise(int* estado) {      //verifica se tem alguma posição que não tem nenhuma possibilidade
    int count;

    for (int i = 0; i < 81; i++) {
        count = 0;
        if (estado[i] == 0) {
            for (int k = 1; k <= 9; k++)
                if (valido(estado, k, i)) count++;
            if (count == 0) return 0;
        }
    }

    return 1;
}

int backtracking(int* estado) {
    if (verifica(estado)) {      // se o jogo acabou imprime estado, limpa a memoria
        imprimir(estado);        // e retorna 1
        free(estado);
        return 1;
    }

    int pos = heu(estado);    //preenche a posição com menos possibilidades

    if (pos > -1) {
        for (int i = 1; i <= 9; i++) {
            if (valido(estado, i, pos)) {
                estado[pos] = i;

                if (Revise(estado))     //se for arco consistente chama pra proxima posicao
                    if (backtracking(estado)) return 1;

                estado[pos] = 0;
            }
        }
    }
    return 0; 
}


int main(int argc, char* argv[]) {
    FILE *f;

    char* arquivo = argv[1];

    f = fopen(arquivo, "r"); 

    if (f == NULL) {
        printf("Erro na abertura do arquivo\n");
        return 0;
    }
    
    char aux[100];

    while(fgets(aux, 100, f) != NULL) {
        int * estado = malloc(sizeof(int) * 81);

        transform(aux, estado);

        backtracking(estado);
    }

    fclose(f);

    return 0;
}