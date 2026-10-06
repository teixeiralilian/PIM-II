#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUTOS 100
#define MAX_VENDEDORES 10

typedef struct {
    int idProduto;
    char nome[50];
    char tamanho[3]; //ex: P, GG, 42, 38...
    float preco;
    int emEstoque;
    int estoqueMin;
    int estoqueMax;
} Produto;

typedef struct {
    int idVendedor;
    char cpf[12];
    char nome[50];
    char telefone[12];
} Vendedor;

Produto produtos[MAX_PRODUTOS];
Vendedor vendedores[MAX_VENDEDORES];

int totalProdutos = 0;
int totalVendedores = 0;

//carrega informações de produtos.txt em Produto produtos[] (caso tenha info)
void carregaProduto() {

    FILE *fp = fopen("produtos.txt", "r");

    if (fp == NULL) {
        printf("Não foi possível carregar o arquivo.");
        return;
    }
    while (totalProdutos < MAX_PRODUTOS &&
        fscanf(fp, " %49[^;];%2[^;];%f;%d;%d;%d", //lê (tamanho do array de char - 1) caracteres para ter espaço pro \0
            produtos[totalProdutos].nome,
            produtos[totalProdutos].tamanho,
            &produtos[totalProdutos].preco,
            &produtos[totalProdutos].emEstoque,
            &produtos[totalProdutos].estoqueMin,
            &produtos[totalProdutos].estoqueMax) == 6) {
            produtos[totalProdutos].idProduto = totalProdutos + 1;
            totalProdutos++;
        }
    fclose(fp);
}

//salva dados de Produto produtos[] em produtos.txt
void salvaProduto() {
    FILE *fp = fopen("produtos.txt", "w"); //reescreve o arquivo todo
    int i;

    if (fp == NULL) {
        printf("Não foi possível salvar.\n");
        return;
    }
    for (i = 0; i < totalProdutos; i++) {
        fprintf(fp, "%s;%s;%.2f;%d;%d;%d\n",
                produtos[i].nome, produtos[i].tamanho, produtos[i].preco,
                produtos[i].emEstoque, produtos[i].estoqueMin, produtos[i].estoqueMax);
    }
    fclose(fp);
}

//recebe info de um produto
void cadastraProduto() {
    printf("\nNome: ");
    fgets(produtos[totalProdutos].nome, sizeof(produtos[totalProdutos].nome),stdin);
    printf("\nTamanho: ");
    scanf("%2s", produtos[totalProdutos].tamanho);
    printf("\nPreço: ");
    scanf("%f", &produtos[totalProdutos].preco);
    printf("\nQuantidade: ");
    scanf("%d", &produtos[totalProdutos].emEstoque);
    printf("\nEstoque mínimo: ");
    scanf("%d", &produtos[totalProdutos].estoqueMin);
    printf("\nEstoque máximo: ");
    scanf("%d", &produtos[totalProdutos].estoqueMax);
    totalProdutos++;
}

//verifica estoque mínimo
void avisoEstoque() {
    for (int i = 0; i < totalProdutos; i++) {
        if (produtos[i].emEstoque <= produtos[i].estoqueMin) {
            printf("=======Estoque mínimo atingido! Hora de reestocar %s!=======\n", produtos[i].nome);
        }
    }
}

//imprime informações dos produtos em Produto produtos[]
void mostraEstoque() {
    int i = 0;

    if (totalProdutos == 0) {
        printf("Nenhum produto catalogado.\n");
        return;
    }
    printf("\n%-5s %-20s %-6s %9s %9s %10s %10s\n",
        "ID", "Nome", "Tamanho", "Preço", "Estoque", "Mín", "Máx");
    for (i = 0; i < totalProdutos; i++) {
        printf("%-5d %-20s %-6s %9.2f %9d %9d %9d\n", i + 1, produtos[i].nome,
        produtos[i].tamanho, produtos[i].preco, produtos[i].emEstoque,
        produtos[i].estoqueMin, produtos[i].estoqueMax);
    }
    avisoEstoque();
}

//carrega info de vendedores.txt
void carregaVendedor() {

    FILE *fp = fopen("vendedores.txt", "r");

    if (fp == NULL) {
        printf("Não foi possível carregar o arquivo.");
        return;
    }
    while (totalVendedores < MAX_VENDEDORES &&
        fscanf(fp, " %49[^;];%11[^;];%11[^;\r\n]",
            vendedores[totalVendedores].nome,
            vendedores[totalVendedores].cpf,
            vendedores[totalVendedores].telefone) == 3) {
            vendedores[totalVendedores].idVendedor = totalVendedores + 1;
            totalVendedores++;
        }
    fclose(fp);
}

//cadastra vendedor
void cadastraVendedor() {
    printf("\nNome: ");
    scanf(" %49[^\n]", vendedores[totalVendedores].nome);
    printf("\nCPF: ");
    scanf(" %11[^\n]", vendedores[totalVendedores].cpf);
    printf("\nTelefone: ");
    scanf(" %11[^\n]", vendedores[totalVendedores].telefone);
    totalVendedores++;
}

//salva dados de vendedores[] em vendedores.txt
void salvaVendedor() {
    FILE *fp = fopen("vendedores.txt", "w");
    int i;

    if (fp == NULL) {
        printf("Não foi possível salvar.\n");
        return;
    }
    for (i = 0; i < totalVendedores; i++) {
        fprintf(fp, "%s;%s;%s\n",
                vendedores[i].nome, vendedores[i].cpf, vendedores[i].telefone);
    }
    fclose(fp);
}

//imprime vendedores registrados em vendedores.txt
void mostraVendedor() {
    int i = 0;

    if (totalVendedores == 0) {
        printf("Nenhum vendedor catalogado.\n");
        return;
    }
    printf("\n%-5s %-30s %-20s %s\n",
        "ID", "Nome", "CPF", "Telefone");
    for (i = 0; i < totalVendedores; i++) {
        printf("%-5d %-30s %-20s %s\n", i + 1,
        vendedores[i].nome, vendedores[i].cpf, vendedores[i].telefone);
    }
}
