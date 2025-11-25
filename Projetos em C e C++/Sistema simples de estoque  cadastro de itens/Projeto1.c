#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Estrutura do item
typedef struct {
    int codigo;         
    char nome[30];      
    float preco;        
    int ativo;          // 1 = ativo | 0 = excluído
} Item;

// -------------------------------------------------------------
// pausa universal (funciona em Linux, Windows e macOS)
void pausa() {
    printf("\nPressione ENTER para continuar...");
    getchar();
    getchar(); 
}

// -------------------------------------------------------------
// retorna quantidade de registros gravados no arquivo
int tamanho(FILE *arq) {
    long atual = ftell(arq);
    fseek(arq, 0, SEEK_END);
    long bytes = ftell(arq);
    fseek(arq, atual, SEEK_SET);
    return bytes / sizeof(Item);
}

// -------------------------------------------------------------
// lê string com segurança usando fgets()
void lerString(char *str, int tam) {
    fflush(stdin);
    fgets(str, tam, stdin);
    str[strcspn(str, "\n")] = '\0';  // remove \n
}

// -------------------------------------------------------------
// cadastra novo item
void cadastrar(FILE *arq) {
    Item x;
    char confirma;

    printf("\n--- Cadastro de Item ---\n");

    x.codigo = tamanho(arq) + 1;

    printf("Código.......: %d\n", x.codigo);

    printf("Nome.........: ");
    lerString(x.nome, sizeof(x.nome));

    printf("Preço........: ");
    scanf("%f", &x.preco);

    x.ativo = 1;

    printf("Confirmar cadastro (s/n)? ");
    fflush(stdin);
    scanf("%c", &confirma);

    if (toupper(confirma) == 'S') {
        fseek(arq, 0, SEEK_END);
        fwrite(&x, sizeof(Item), 1, arq);
        printf("\nItem gravado com sucesso!\n");
    } else {
        printf("\nCadastro cancelado.\n");
    }

    pausa();
}

// -------------------------------------------------------------
// consulta item por código
void consultar(FILE *arq) {
    Item x;
    int cod;

    printf("\n--- Consulta de Item ---\n");
    printf("Informe o código do item: ");
    scanf("%d", &cod);

    if (cod <= 0 || cod > tamanho(arq)) {
        printf("\nCódigo inválido!\n");
    } else {
        fseek(arq, (cod - 1) * sizeof(Item), SEEK_SET);
        fread(&x, sizeof(Item), 1, arq);

        if (x.ativo == 0) {
            printf("\nEste item está excluído.\n");
        } else {
            printf("\nCódigo: %d\n", x.codigo);
            printf("Nome..: %s\n", x.nome);
            printf("Preço.: %.2f\n", x.preco);
        }
    }

    pausa();
}

// -------------------------------------------------------------
// exclusão lógica: apenas marca como inativo
void excluir(FILE *arq) {
    Item x;
    int cod;
    char confirma;

    printf("\n--- Excluir Item ---\n");
    printf("Informe o código do item: ");
    scanf("%d", &cod);

    if (cod <= 0 || cod > tamanho(arq)) {
        printf("\nCódigo inválido!\n");
    } else {
        fseek(arq, (cod - 1) * sizeof(Item), SEEK_SET);
        fread(&x, sizeof(Item), 1, arq);

        if (x.ativo == 0) {
            printf("\nEste item já está excluído.\n");
        } else {
            printf("\nItem encontrado:\n");
            printf("Nome: %s\n", x.nome);
            printf("Preço: %.2f\n", x.preco);

            printf("Confirmar exclusão (s/n)? ");
            fflush(stdin);
            scanf("%c", &confirma);

            if (toupper(confirma) == 'S') {
                x.ativo = 0;
                fseek(arq, (cod - 1) * sizeof(Item), SEEK_SET);
                fwrite(&x, sizeof(Item), 1, arq);
                printf("\nItem excluído com sucesso!\n");
            }
        }
    }

    pausa();
}

// -------------------------------------------------------------
// gera relatório em arquivo TXT
void gerarRelatorio(FILE *arq) {
    FILE *txt;
    Item x;

    txt = fopen("relatorio.txt", "w");
    if (txt == NULL) {
        printf("\nErro ao criar relatorio.txt\n");
        return;
    }

    rewind(arq);

    fprintf(txt, "RELATÓRIO DE ITENS ATIVOS\n");
    fprintf(txt, "----------------------------\n\n");

    while (!feof(arq)) {
        fread(&x, sizeof(Item), 1, arq);

        if (!feof(arq) && x.ativo == 1) {
            fprintf(txt, "Código: %d\n", x.codigo);
            fprintf(txt, "Nome..: %s\n", x.nome);
            fprintf(txt, "Preço.: %.2f\n\n", x.preco);
        }
    }

    fclose(txt);

    printf("\nRelatório gerado com sucesso: relatorio.txt\n");

    pausa();
}

// -------------------------------------------------------------
// menu principal
int main() {
    FILE *arq;

    // abre o arquivo binário
    arq = fopen("itens.dat", "rb+");
    if (arq == NULL) {
        arq = fopen("itens.dat", "wb+");
        if (arq == NULL) {
            printf("Erro ao abrir arquivo!\n");
            return 1;
        }
    }

    int op;

    do {
        printf("\n===== SISTEMA DE CADASTRO DE ITENS =====\n");
        printf("1 - Cadastrar item\n");
        printf("2 - Consultar item\n");
        printf("3 - Excluir item\n");
        printf("4 - Gerar relatório (.txt)\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &op);

        switch (op) {
            case 1: cadastrar(arq); break;
            case 2: consultar(arq); break;
            case 3: excluir(arq); break;
            case 4: gerarRelatorio(arq); break;
            case 0: break;
            default: printf("\nOpção inválida!\n"); pausa();
        }

    } while (op != 0);

    fclose(arq);

    return 0;
}
