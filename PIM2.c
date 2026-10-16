// ==============================================
// SISTEMA DE VENDAS - HIERARQUIA DE ACESSO
// ==============================================

// ===== BIBLIOTECAS PADRÃO =====
#include <stdio.h>      // Entrada e saída de dados
#include <string.h>     // Manipulação de textos
#include <stdlib.h>     // Funções gerais (system, malloc, etc)

// ===== BIBLIOTECAS PARA EXIBIR PORTUGUÊS CORRETAMENTE =====
#include <locale.h>     // Define idioma e codificação
#ifdef _WIN32            // Se estiver no Windows
#include <windows.h>    // Ajusta configurações do console
#endif

// ==============================================
// DEFINIÇÃO DOS NÍVEIS DE ACESSO
// Quanto menor o número, mais permissões
// ==============================================
typedef enum {
    ADMINISTRADOR = 1,   // Nível 1 → pode TUDO
    GERENTE,              // Nível 2 → funções administrativas, exceto cadastrar gerente
    VENDEDOR              // Nível 3 → funções básicas de venda
} NivelAcesso;

// ==============================================
// ESTRUTURAS DE DADOS
// Organizam as informações em "caixinhas" nomeadas
// ==============================================

// Armazena os dados de cada pessoa do sistema
typedef struct {
    char nome[50];         // Nome completo da pessoa
    char usuario[30];      // Login para entrar no sistema
    char senha[30];        // Senha de acesso
    char telefone[15];     // Telefone de contato
    NivelAcesso nivel;     // Nível de permissão (1, 2 ou 3)
    int ativo;             // 1 = ativo, 0 = arquivado
} Usuario;

// Armazena os dados de cada produto
typedef struct {
    char nome[50];         // Nome do produto
    char tamanho[10];      // Tamanho do produto (ex: P, M, G, 42, 38...)
    float preco;           // Preço de uma unidade
    int quantidade;        // Quantidade disponível no estoque
    int estoqueMin;        // Estoque mínimo
    int estoqueMax;        // Estoque máximo
    int ativo;             // 1 = ativo, 0 = arquivado
} Produto;

// Armazena os dados de cada venda realizada
typedef struct {
    char vendedor[50];     // Nome de quem vendeu
    char produto[50];      // Nome do produto vendido
    int quantidade;        // Quantidade de unidades vendidas
    float valorTotal;      // Preço × quantidade
    char data[11];          // Data da venda (dd/mm/aaaa)
} Venda;

// ==============================================
// VARIÁVEIS GLOBAIS
// Podem ser usadas por QUALQUER função do código
// ==============================================
Usuario usuarios[100];     // Lista onde ficam guardados todos os usuários
int qtdUsuarios = 0;        // Conta quantos usuários já foram cadastrados

Produto produtos[100];      // Lista de produtos do estoque
int qtdProdutos = 0;        // Conta quantos produtos existem

Venda vendas[200];          // Lista de todas as vendas já feitas
int qtdVendas = 0;          // Conta quantas vendas foram registradas

Usuario usuarioLogado;      // Guarda os dados da pessoa que está usando o sistema agora

// ===== NOMES DOS ARQUIVOS =====
const char* ARQ_USUARIOS = "usuarios.txt";  // Arquivo que guarda os usuários
const char* ARQ_PRODUTOS = "produtos.txt";  // Arquivo que guarda os produtos
const char* ARQ_VENDAS   = "vendas.txt";   // Arquivo que guarda as vendas

// ==============================================
// FUNÇÃO: CONFIGURAR IDIOMA
// Ajusta o programa para exibir acentos e ç corretamente
// ==============================================
void configurarIdioma() {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");  // Configuração para Windows
    setlocale(LC_ALL, "pt_BR.UTF-8");             // Configuração para Linux/macOS

    // Ajustes específicos só para o Windows
    #ifdef _WIN32
    SetConsoleCP(65001);           // Define entrada de texto em UTF-8
    SetConsoleOutputCP(65001);     // Define saída de texto em UTF-8
    system("chcp 65001 > nul");    // Muda página de código para UTF-8
    #endif
}

// ==============================================
// FUNÇÃO: LIMPAR TELA
// Apaga tudo que está na tela para um visual mais limpo
// ==============================================
void limparTela() {
    #ifdef _WIN32
    system("cls");                  // Comando para limpar no Windows
    #else
    system("clear");                // Comando para limpar no Linux/macOS
    #endif
}

// ==============================================
// FUNÇÃO: SALVAR USUÁRIOS NO ARQUIVO
// Pega a lista de usuários e grava tudo no arquivo usuarios.txt
// ==============================================
void salvarUsuarios() {
    FILE* arquivo = fopen(ARQ_USUARIOS, "w");  // Abre arquivo para escrita
    if (arquivo == NULL) {                             // Se não conseguiu abrir
        printf("ERRO: Não foi possível salvar usuários!\n");
        return;
    }
    // Percorre todos os usuários e grava cada um no arquivo
    for (int i = 0; i < qtdUsuarios; i++) {
        fprintf(arquivo, "%s;%s;%s;%d;%s;%d\n",
                usuarios[i].nome,
                usuarios[i].usuario,
                usuarios[i].senha,
                usuarios[i].nivel,
                usuarios[i].telefone,
                usuarios[i].ativo);
    }
    fclose(arquivo);  // Fecha o arquivo (importante para não perder dados)
}

// ==============================================
// FUNÇÃO: SALVAR PRODUTOS NO ARQUIVO
// Grava todos os produtos e estoque no arquivo produtos.txt
// ==============================================
void salvarProdutos() {
    FILE* arquivo = fopen(ARQ_PRODUTOS, "w");
    if (arquivo == NULL) {
        printf("ERRO: Não foi possível salvar produtos!\n");
        return;
    }
    for (int i = 0; i < qtdProdutos; i++) {
        fprintf(arquivo, "%s;%s;%.2f;%d;%d;%d;%d\n",
                produtos[i].nome,
                produtos[i].tamanho,
                produtos[i].preco,
                produtos[i].quantidade,
                produtos[i].estoqueMin,
                produtos[i].estoqueMax,
                produtos[i].ativo);
    }
    fclose(arquivo);
}

// ==============================================
// FUNÇÃO: SALVAR VENDAS NO ARQUIVO
// Grava todo o histórico de vendas no arquivo vendas.txt
// ==============================================
void salvarVendas() {
    FILE* arquivo = fopen(ARQ_VENDAS, "w");
    if (arquivo == NULL) {
        printf("ERRO: Não foi possível salvar vendas!\n");
        return;
    }
    for (int i = 0; i < qtdVendas; i++) {
        fprintf(arquivo, "%s;%s;%d;%.2f;%s\n",
                vendas[i].vendedor,
                vendas[i].produto,
                vendas[i].quantidade,
                vendas[i].valorTotal,
                vendas[i].data);
    }
    fclose(arquivo);
}

// ==============================================
// FUNÇÃO: SALVAR TUDO
// Chama as 3 funções de salvamento de uma vez
// ==============================================
void salvarTudo() {
    salvarUsuarios();
    salvarProdutos();
    salvarVendas();
}

// ==============================================
// FUNÇÃO: CARREGAR USUÁRIOS
// Lê o arquivo usuarios.txt e traz os dados de volta para o programa
// ==============================================
void carregarUsuarios() {
    FILE* arquivo = fopen(ARQ_USUARIOS, "r");  // Abre arquivo para leitura

    // Se o arquivo não existir (primeira vez abrindo o programa)
    if (arquivo == NULL) {
        // Cria o administrador padrão
        strcpy(usuarios[0].nome, "Administrador Geral");
        strcpy(usuarios[0].usuario, "admin");
        strcpy(usuarios[0].senha, "123456");
        strcpy(usuarios[0].telefone, "00000000000");
        usuarios[0].ativo = 1;
        usuarios[0].nivel = ADMINISTRADOR;
        qtdUsuarios = 1;
        salvarUsuarios();  // Cria o arquivo com o admin padrão
        return;
    }

    // Lê cada linha do arquivo e coloca na lista de usuários
    while (fscanf(arquivo, "%[^;];%[^;];%[^;];%d;%14[^;];%d\n",
                  usuarios[qtdUsuarios].nome,
                  usuarios[qtdUsuarios].usuario,
                  usuarios[qtdUsuarios].senha,
                  &usuarios[qtdUsuarios].nivel,
                  usuarios[qtdUsuarios].telefone,
                  &usuarios[qtdUsuarios].ativo) == 6) {
        qtdUsuarios++;  // Conta mais um usuário carregado
    }
    fclose(arquivo);
}

// ==============================================
// FUNÇÃO: CARREGAR PRODUTOS
// Lê o arquivo produtos.txt e traz os produtos de volta
// ==============================================
void carregarProdutos() {
    FILE* arquivo = fopen(ARQ_PRODUTOS, "r");
    if (arquivo == NULL) return;  // Se não existe, não tem o que carregar

    while (fscanf(arquivo, "%[^;];%[^;];%f;%d;%d;%d;%d\n",
                  produtos[qtdProdutos].nome,
                  produtos[qtdProdutos].tamanho,
                  &produtos[qtdProdutos].preco,
                  &produtos[qtdProdutos].quantidade,
                  &produtos[qtdProdutos].estoqueMin,
                  &produtos[qtdProdutos].estoqueMax,
                  &produtos[qtdProdutos].ativo) == 7) {
        qtdProdutos++;
    }
    fclose(arquivo);
}

// ==============================================
// FUNÇÃO: CARREGAR VENDAS
// Lê o arquivo vendas.txt e traz o histórico de volta
// ==============================================
void carregarVendas() {
    FILE* arquivo = fopen(ARQ_VENDAS, "r");
    if (arquivo == NULL){
        return;
    }

    while (fscanf(arquivo, "%[^;];%[^;];%d;%f;%s\n",
                  vendas[qtdVendas].vendedor,
                  vendas[qtdVendas].produto,
                  &vendas[qtdVendas].quantidade,
                  &vendas[qtdVendas].valorTotal,
                  vendas[qtdVendas].data) == 5) {
        qtdVendas++;
    }
    fclose(arquivo);
}

// ==============================================
// FUNÇÃO: CARREGAR TUDO
// Chama as 3 funções de carregamento de uma vez
// ==============================================
void carregarTudo() {
    carregarUsuarios();
    carregarProdutos();
    carregarVendas();
}

// ==============================================
// FUNÇÃO: VERIFICAR PERMISSÃO
// Compara o nível do usuário logado com o nível necessário
// Se o nível do usuário for menor ou igual → pode acessar
// ==============================================
int temPermissao(NivelAcesso nivelMinimoNecessario) {
    return usuarioLogado.nivel <= nivelMinimoNecessario;
}

// ==============================================
// FUNÇÃO: LOGIN
// Pede usuário e senha, verifica se estão corretos
// Retorna 1 se deu certo, 0 se deu errado
// ==============================================
int login() {
    char user[30], senha[30];

    printf("========== SISTEMA DE VENDAS ==========\n");
    printf("Digite seu usuário: ");
    scanf("%29s", user);
    printf("Digite sua senha: ");
    scanf("%29s", senha);

    // Percorre a lista procurando usuário e senha iguais
    for (int i = 0; i < qtdUsuarios; i++) {
        if (strcmp(usuarios[i].usuario, user) == 0 &&
            strcmp(usuarios[i].senha, senha) == 0 &&
            usuarios[i].ativo == 1) {  // Só permite logar se estiver ativo 
            usuarioLogado = usuarios[i];  // Salva quem entrou
            printf("\nBem-vindo, %s!\n", usuarioLogado.nome);
            return 1;  // Login OK
        }
    }

    printf("\nUsuário ou senha incorretos!\n");
    return 0;  // Login falhou
}

// ==============================================
// FUNÇÃO: VERIFICAR SE O LOGIN JÁ EXISTE
// indiceIgnorado = posição do usuário que está sendo editado
// (use -1 quando for um cadastro novo)
// ==============================================
int loginJaExiste(const char* login, int indiceIgnorado) {
    for (int i = 0; i < qtdUsuarios; i++) {
        if (i != indiceIgnorado && strcmp(usuarios[i].usuario, login) == 0) {
            return 1;
        }
    }
    return 0;
}

// ==============================================
// FUNÇÃO: VER TODOS OS USUÁRIOS
// Mostra uma lista com todos cadastrados
// Disponível só para Administrador e Gerente
// ==============================================
void verTodosUsuarios() {
    // Verifica permissão antes de mostrar
    if (!temPermissao(GERENTE)) {
        printf("Acesso negado! Apenas Administrador e Gerente podem ver os registros.\n");
        return;
    }

    printf("\n========== USUÁRIOS CADASTRADOS ==========\n");
    if (qtdUsuarios == 0) {
        printf("Nenhum usuário cadastrado.\n");
        return;
    }

    // Mostra cada usuário com número
    for (int i = 0; i < qtdUsuarios; i++) {
        printf("[%d] Nome: %-20s | Usuário: %-15s | Nível: ",
               i + 1, usuarios[i].nome, usuarios[i].usuario);

        switch (usuarios[i].nivel) {
        case ADMINISTRADOR:
            printf("Administrador\n");
            break;
        case GERENTE:
            printf("Gerente\n");
            break;
        case VENDEDOR:
            printf("Vendedor\n");
            break;
        }
    }
}

// ==============================================
// FUNÇÃO: EDITAR USUÁRIO
// Permite alterar dados de um usuário existente
// Gerente não pode editar Administrador
// Apenas Administrador pode mudar o nível de acesso
// ==============================================
void editarUsuario() {
    if (!temPermissao(GERENTE)) {
        printf("Acesso negado! Apenas Administrador e Gerente podem editar.\n");
        return;
    }

    verTodosUsuarios();  // Mostra a lista primeiro para a pessoa escolher

    int numeroEscolhido = -1;
    printf("\nDigite o número do usuário que deseja editar: ");
    if (scanf("%d", &numeroEscolhido) != 1) {
        printf("Entrada inválida!\n");
        return;
    }
    numeroEscolhido--;  // Converte para posição da lista (começa em 0)

    // Verifica se o número existe na lista
    if (numeroEscolhido < 0 || numeroEscolhido >= qtdUsuarios) {
        printf("Número inválido!\n");
        return;
    }

    // Regra de segurança: Gerente não mexe em Administrador
    if (usuarioLogado.nivel == GERENTE && usuarios[numeroEscolhido].nivel == ADMINISTRADOR) {
        printf("Acesso negado! Gerente não pode editar um Administrador.\n");
        return;
    }

    // Guarda os valores antigos para usar no final
    char loginAntigo[30], nomeAntigo[50];
    strcpy(loginAntigo, usuarios[numeroEscolhido].usuario);
    strcpy(nomeAntigo, usuarios[numeroEscolhido].nome);

    char texto[50];      // Guarda o que a pessoa digitar
    int novoNivel = -1;

    printf("\n--- Editando: %s ---\n", usuarios[numeroEscolhido].nome);
    printf("→ Digite . (ponto) para MANTER o valor atual\n");

    // Nome
    printf("Novo nome (. para manter) [%s]: ", usuarios[numeroEscolhido].nome);
    scanf(" %49[^\n]", texto);
    if (strcmp(texto, ".") != 0)
        strcpy(usuarios[numeroEscolhido].nome, texto);

    // Usuário (login) não pode repetir o de outra pessoa
    printf("Novo usuário (. para manter) [%s]: ", usuarios[numeroEscolhido].usuario);
    scanf("%29s", texto);
    if (strcmp(texto, ".") != 0) {
        if (loginJaExiste(texto, numeroEscolhido))
            printf("Esse usuário já existe! Mantendo o atual.\n");
        else
            strcpy(usuarios[numeroEscolhido].usuario, texto);
    }

    // Telefone
    printf("Novo telefone (. para manter) [%s]: ", usuarios[numeroEscolhido].telefone);
    scanf("%14s", texto);
    if (strcmp(texto, ".") != 0)
        strcpy(usuarios[numeroEscolhido].telefone, texto);

    // Senha
    printf("Nova senha (. para manter): ");
    scanf("%29s", texto);
    if (strcmp(texto, ".") != 0)
        strcpy(usuarios[numeroEscolhido].senha, texto);

    // Nível de acesso: só Administrador pode mudar
    if (usuarioLogado.nivel == ADMINISTRADOR) {
        printf("Novo nível (1=Admin, 2=Gerente, 3=Vendedor, -1 para manter) [%d]: ", usuarios[numeroEscolhido].nivel);
        scanf("%d", &novoNivel);
        if (novoNivel >= 1 && novoNivel <= 3)
            usuarios[numeroEscolhido].nivel = novoNivel;
    }

    // Se o nome mudou, atualiza as vendas antigas desse vendedor
    if (strcmp(nomeAntigo, usuarios[numeroEscolhido].nome) != 0) {
        for (int i = 0; i < qtdVendas; i++) {
            if (strcmp(vendas[i].vendedor, nomeAntigo) == 0)
                strcpy(vendas[i].vendedor, usuarios[numeroEscolhido].nome);
        }
        salvarVendas();
    }

    // Se a pessoa editou a si mesma, atualiza a cópia de quem está logado
    if (strcmp(loginAntigo, usuarioLogado.usuario) == 0)
        usuarioLogado = usuarios[numeroEscolhido];

    salvarUsuarios();  // Salva as alterações no arquivo
    printf("Usuário atualizado com sucesso!\n");
}

// ==============================================
// FUNÇÃO: CADASTRAR VENDEDOR
// Cria um novo usuário com nível de Vendedor
// Disponível para Gerente e Administrador
// ==============================================
void cadastrarVendedor() {
    if (!temPermissao(GERENTE)) {
        printf("Acesso negado!\n");
        return;
    }

    if (qtdUsuarios >= 100) {
        printf("Limite de usuários atingido!\n");
        return;
    }

    Usuario novo;
    printf("\n--- Cadastro de Vendedor ---\n");
    printf("Nome completo: ");
    scanf(" %49[^\n]", novo.nome);
    printf("Usuário de acesso: ");
    scanf("%29s", novo.usuario);
    if (loginJaExiste(novo.usuario, -1)) {
        printf("Esse usuário já existe!\n");
        return;
    }
    printf("Senha: ");
    scanf("%29s", novo.senha);
    printf("Telefone: ");
    scanf("%14s", novo.telefone);
    novo.ativo = 1;  // Define como ativo

    novo.nivel = VENDEDOR;  // Define que é vendedor

    usuarios[qtdUsuarios] = novo;  // Adiciona na lista
    qtdUsuarios++;
    salvarUsuarios();  // Atualiza arquivo
    printf("Vendedor cadastrado e salvo!\n");
}

// ==============================================
// FUNÇÃO: CADASTRAR GERENTE
// Cria um novo usuário com nível de Gerente
// Disponível SOMENTE para Administrador
// ==============================================
void cadastrarGerente() {
    if (usuarioLogado.nivel != ADMINISTRADOR) {
        printf("Acesso negado! Apenas Administrador pode cadastrar Gerente.\n");
        return;
    }

    if (qtdUsuarios >= 100) {
        printf("Limite de usuários atingido!\n");
        return;
    }

    Usuario novo;
    printf("\n--- Cadastro de Gerente ---\n");
    printf("Nome completo: ");
    scanf(" %49[^\n]", novo.nome);
    printf("Usuário de acesso: ");
    scanf("%29s", novo.usuario);
    if (loginJaExiste(novo.usuario, -1)) {
        printf("Esse usuário já existe!\n");
        return;
    }
    printf("Senha: ");
    scanf("%29s", novo.senha);
    printf("Telefone: ");
    scanf("%14s", novo.telefone);
    novo.nivel = GERENTE;
    novo.ativo = 1;  // Define como ativo

    usuarios[qtdUsuarios] = novo;
    qtdUsuarios++;
    salvarUsuarios();
    printf("Gerente cadastrado e salvo!\n");
}

// ==============================================
// FUNÇÃO: CADASTRAR PRODUTO
// Adiciona um produto novo no estoque
// Disponível para Gerente e Administrador
// ==============================================
void cadastrarProduto() { //add estoque min e max e aviso de estoque min atingido
    if (!temPermissao(GERENTE)) {
        printf("Acesso negado!\n");
        return;
    }

    if (qtdProdutos >= 100) {
        printf("Limite de produtos atingido!\n");
        return;
    }

    Produto p;
    printf("\n--- Cadastro de Produto ---\n");
    printf("Nome do produto: ");
    scanf(" %49[^\n]", p.nome);
    printf("Tamanho (ex: P, M, G, 42, 38...): ");
    scanf("%9s", p.tamanho);
    printf("Preço unitário: R$ ");
    scanf("%f", &p.preco);
    printf("Quantidade em estoque: ");
    scanf("%d", &p.quantidade);
    printf("Estoque mínimo: ");
    scanf("%d", &p.estoqueMin); 
    printf("Estoque máximo: ");
    scanf("%d", &p.estoqueMax);

    // Não aceita valores sem sentido
    if (p.preco < 0 || p.quantidade < 0 || p.estoqueMin < 0 || p.estoqueMax < p.estoqueMin) {
        printf("Valores inválidos! Nada pode ser negativo e o máximo não pode ser menor que o mínimo.\n");
        return;
    }

    p.ativo = 1;  // Define como ativo

    produtos[qtdProdutos] = p;
    qtdProdutos++;
    salvarProdutos();
    printf("Produto cadastrado e salvo!\n");
}

// ==============================================
// FUNÇÃO: CONSULTAR ESTOQUE
// Mostra todos os produtos disponíveis
// Liberado para todos os níveis
// ==============================================
void consultarEstoque() {
    printf("\n========== ESTOQUE ==========\n");
    if (qtdProdutos == 0) {
        printf("Estoque vazio.\n");
        return;
    }

    for (int i = 0; i < qtdProdutos; i++) {
        if (produtos[i].ativo == 0) {
            continue;
        }  
        printf("[%d] %-20s | Tamanho: %-10s | Preço: R$ %8.2f | Estoque: %d unidades",
               i + 1,
               produtos[i].nome,
               produtos[i].tamanho,
               produtos[i].preco,
               produtos[i].quantidade);
        if (produtos[i].quantidade <= produtos[i].estoqueMin) {
            printf(" | ATENÇÃO: Estoque mínimo atingido!");
        }
        printf("\n");
    }
}

// ==============================================
// FUNÇÃO: ARQUIVAR / RESTAURAR USUÁRIO
// Se estiver ativo, arquiva. Se estiver arquivado, restaura.
// Disponível para Gerente e Administrador
// ==============================================
void arquivarUsuario() {
    if (!temPermissao(GERENTE)) {
        printf("Acesso negado! Apenas Administrador e Gerente podem arquivar usuários.\n");
        return;
    }

    printf("\n========== USUÁRIOS ==========\n");
    for (int i = 0; i < qtdUsuarios; i++) {
        printf("[%d] %-20s | Usuário: %-15s", i + 1, usuarios[i].nome, usuarios[i].usuario);
        if (usuarios[i].ativo == 0)
            printf(" | ARQUIVADO");
        printf("\n");
    }

    int numero = -1;
    printf("\nDigite o número do usuário: ");
    if (scanf("%d", &numero) != 1) {
        printf("Entrada inválida!\n");
        while (getchar() != '\n');   // Descarta o que foi digitado errado
        return;
    }
    numero--;

    if (numero < 0 || numero >= qtdUsuarios) {
        printf("Número inválido!\n");
        return;
    }

    // Ninguém arquiva a si mesmo (garante que sempre sobra alguém com acesso)
    if (strcmp(usuarios[numero].usuario, usuarioLogado.usuario) == 0) {
        printf("Você não pode arquivar a si mesmo!\n");
        return;
    }

    // Gerente não mexe em Administrador
    if (usuarioLogado.nivel == GERENTE && usuarios[numero].nivel == ADMINISTRADOR) {
        printf("Acesso negado! Gerente não pode arquivar um Administrador.\n");
        return;
    }

    if (usuarios[numero].ativo == 1) {
        usuarios[numero].ativo = 0;
        printf("Usuário '%s' arquivado.\n", usuarios[numero].nome);
    } else {
        usuarios[numero].ativo = 1;
        printf("Usuário '%s' restaurado.\n", usuarios[numero].nome);
    }

    salvarUsuarios();
}

// ==============================================
// FUNÇÃO: EDITAR PRODUTO
// Permite alterar dados de um produto existente
// ==============================================
void editarProduto() {
    if (!temPermissao(GERENTE)) {
        printf("Acesso negado! Apenas Administrador e Gerente podem editar produtos.\n");
        return;
    }

    consultarEstoque();

    int numero = -1;
    printf("\nDigite o número do produto que deseja editar: ");
    scanf("%d", &numero);
    numero--;

    if (numero < 0 || numero >= qtdProdutos) {
        printf("Número inválido!\n");
        return;
    }

    char novoNome[50];
    float novoPreco;
    int novaQuantidade;
    int novoEstoqueMin, novoEstoqueMax;

    printf("\n--- Editando: %s ---\n", produtos[numero].nome);

    printf("Novo nome (. para manter) [%s]: ", produtos[numero].nome);
    scanf(" %49[^\n]", novoNome);
    if (strcmp(novoNome, ".") != 0)
        strcpy(produtos[numero].nome, novoNome);

    printf("Novo preço (-1 para manter) [%.2f]: ", produtos[numero].preco);
    scanf("%f", &novoPreco);
    if (novoPreco >= 0)
        produtos[numero].preco = novoPreco;

    printf("Nova quantidade (-1 para manter) [%d]: ", produtos[numero].quantidade);
    scanf("%d", &novaQuantidade);
    if (novaQuantidade >= 0)
        produtos[numero].quantidade = novaQuantidade;

    printf("Novo estoque mínimo (-1 para manter) [%d]: ", produtos[numero].estoqueMin);
    scanf("%d", &novoEstoqueMin);

    printf("Novo estoque máximo (-1 para manter) [%d]: ", produtos[numero].estoqueMax);
    scanf("%d", &novoEstoqueMax);

    // Valores finais de mínimo e máximo (o antigo, se a pessoa digitou -1)
    int minFinal = (novoEstoqueMin >= 0) ? novoEstoqueMin : produtos[numero].estoqueMin;
    int maxFinal = (novoEstoqueMax >= 0) ? novoEstoqueMax : produtos[numero].estoqueMax;

    if (maxFinal < minFinal) {
        printf("Estoque máximo não pode ser menor que o mínimo. Valores de estoque mantidos.\n");
    } else {
        produtos[numero].estoqueMin = minFinal;
        produtos[numero].estoqueMax = maxFinal;
    }

    salvarProdutos();
    printf("Produto atualizado com sucesso!\n");
}

// ==============================================
// FUNÇÃO: ARQUIVAR / RESTAURAR PRODUTO
// Se estiver ativo, arquiva. Se estiver arquivado, restaura.
// Disponível para Gerente e Administrador
// ==============================================
void arquivarProduto() {
    if (!temPermissao(GERENTE)) {
        printf("Acesso negado! Apenas Administrador e Gerente podem arquivar produtos.\n");
        return;
    }

    // Lista TODOS os produtos (o consultarEstoque esconde os arquivados)
    printf("\n========== PRODUTOS ==========\n");
    for (int i = 0; i < qtdProdutos; i++) {
        printf("[%d] %-20s | Estoque: %d", i + 1, produtos[i].nome, produtos[i].quantidade);
        if (produtos[i].ativo == 0)
            printf(" | ARQUIVADO");
        printf("\n");
    }

    int numero = -1;
    printf("\nDigite o número do produto: ");
    if (scanf("%d", &numero) != 1) {
        printf("Entrada inválida!\n");
        while (getchar() != '\n');
        return;
    }
    numero--;

    if (numero < 0 || numero >= qtdProdutos) {
        printf("Número inválido!\n");
        return;
    }

    if (produtos[numero].ativo == 1) {
        produtos[numero].ativo = 0;
        printf("Produto '%s' arquivado.\n", produtos[numero].nome);
    } else {
        produtos[numero].ativo = 1;
        printf("Produto '%s' restaurado.\n", produtos[numero].nome);
    }

    salvarProdutos();
}

// ==============================================
// FUNÇÃO: REGISTRAR VENDA
// Diminui quantidade do estoque e guarda a venda
// Liberado para todos os níveis
// ==============================================
void registrarVenda() {
    if (!temPermissao(VENDEDOR)) {
        printf("Acesso negado!\n");
        return;
    }

    if (qtdVendas >= 200) {
        printf("Limite de vendas atingido!\n");
        return;
    }

    char nomeProduto[50];
    char dataVenda[11];
    int quantidadeVendida;

    printf("\n--- Registrar Venda ---\n");
    printf("Nome do produto vendido: ");
    scanf(" %49[^\n]", nomeProduto);

    // Procura o produto pelo nome
    int indiceEncontrado = -1;
    for (int i = 0; i < qtdProdutos; i++) {
        if (strcmp(produtos[i].nome, nomeProduto) == 0 && produtos[i].ativo == 1) {
            indiceEncontrado = i;
            break;
        }
    }

    if (indiceEncontrado == -1) {
        printf("Produto não encontrado!\n");
        return;
    }

    printf("Quantidade vendida: ");
    scanf("%d", &quantidadeVendida);

    // Não aceita quantidade zero ou negativa
    if (quantidadeVendida <= 0) {
        printf("Quantidade inválida!\n");
        return;
    }

    // Verifica se tem estoque suficiente
    if (produtos[indiceEncontrado].quantidade < quantidadeVendida) {
        printf("Estoque insuficiente! Disponível: %d\n", produtos[indiceEncontrado].quantidade);
        return;
    }

    printf("Data da venda (dd/mm/aaaa): ");
    scanf(" %10s", dataVenda);

    // Diminui do estoque
    produtos[indiceEncontrado].quantidade -= quantidadeVendida;

    // Cria o registro da venda
    Venda novaVenda;
    strcpy(novaVenda.vendedor, usuarioLogado.nome);
    strcpy(novaVenda.produto, produtos[indiceEncontrado].nome);
    novaVenda.quantidade = quantidadeVendida;
    novaVenda.valorTotal = quantidadeVendida * produtos[indiceEncontrado].preco;
    strcpy(novaVenda.data, dataVenda);
    vendas[qtdVendas] = novaVenda;
    qtdVendas++;

    salvarProdutos();  // Atualiza estoque no arquivo
    salvarVendas();   // Salva a venda no arquivo

    printf("Venda registrada! Valor total: R$ %.2f\n", novaVenda.valorTotal);
}

// ==============================================
// FUNÇÃO: CONSULTAR VENDAS POR VENDEDOR
// Mostra todas as vendas de uma pessoa específica
// Disponível para Gerente e Administrador
// ==============================================
void consultarVendasPorVendedor() {
    if (!temPermissao(GERENTE)) {
        printf("Acesso negado!\n");
        return;
    }

    char nomeVendedor[50];
    printf("\n--- Consultar Vendas ---\n");
    printf("Nome do vendedor: ");
    scanf(" %49[^\n]", nomeVendedor);

    float totalGeral = 0;
    int encontrou = 0;

    printf("\n--- Vendas encontradas ---\n");
    for (int i = 0; i < qtdVendas; i++) {
        if (strcmp(vendas[i].vendedor, nomeVendedor) == 0) {
            printf("Data: %-10s | Produto: %-20s | Qtd: %3d | Valor: R$ %8.2f\n",
                   vendas[i].data,
                   vendas[i].produto,
                   vendas[i].quantidade,
                   vendas[i].valorTotal);
            totalGeral += vendas[i].valorTotal;
            encontrou = 1;
        }
    }

    if (!encontrou)
        printf("Nenhuma venda encontrada para este vendedor.\n");
    else
        printf("\nTOTAL DE VENDAS: R$ %.2f\n", totalGeral);
}

// ==============================================
// FUNÇÃO: EXIBIR MENU
// Mostra as opções de acordo com o nível de quem está logado
// ==============================================
void exibirMenu() {
    limparTela();  // Limpa antes de mostrar o menu novo

    printf("\n========================================\n");
    printf("Usuário: %-25s | Nível: ", usuarioLogado.nome);
    switch (usuarioLogado.nivel) {
        case ADMINISTRADOR: 
            printf("ADMINISTRADOR\n");
            break;
        case GERENTE:
            printf("GERENTE\n");
            break;
        case VENDEDOR:
            printf("VENDEDOR\n");
            break;
    }
    printf("========================================\n");

    // Opções visíveis para TODOS
    printf("1  - Consultar Estoque\n");
    printf("2  - Registrar Venda\n");

    // Opções visíveis só para Gerente e Administrador
    if (usuarioLogado.nivel == GERENTE || usuarioLogado.nivel == ADMINISTRADOR) {
        printf("3  - Cadastrar Vendedor\n");
        printf("4  - Cadastrar Produto\n");
        printf("5  - Consultar Vendas por Vendedor\n");
        printf("6  - Ver Todos os Usuários\n");
        printf("7  - Editar Usuário\n");
        printf("8  - Editar Produto\n");
        printf("9  - Arquivar/Restaurar Produto\n");
        printf("10 - Arquivar/Restaurar Usuário\n");
    }

    // Opção visível SÓ para Administrador
    if (usuarioLogado.nivel == ADMINISTRADOR) {
        printf("11 - Cadastrar Gerente\n");
    }

    printf("0  - Sair do Sistema\n");
    printf("========================================\n");
    printf("Escolha uma opção: ");
}

// ==============================================
// FUNÇÃO PRINCIPAL
// É o que roda quando o programa abre
// ==============================================
int main() {
    configurarIdioma();  // Ajusta acentos
    carregarTudo();      // Traz dados salvos dos arquivos

    // Tenta logar, se falhar, fecha o programa
    if (!login()) {
        return 1;
    }

    int opcaoEscolhida;

    // ===== ESTRUTURA WHILE SIMPLIFICADA =====
    // Repete o menu enquanto a pessoa não digitar 0
    opcaoEscolhida = -1;  // Valor inicial qualquer que não seja 0
    while (opcaoEscolhida != 0) {
        exibirMenu();
        if (scanf("%d", &opcaoEscolhida) != 1) {
            // Não digitou número: cai em "Opção inválida" (a pausa abaixo limpa o teclado)
            opcaoEscolhida = -1;
        }

        // Executa a função correspondente à opção
        switch (opcaoEscolhida) {
            case 1: 
                consultarEstoque(); 
                break;
            case 2: 
                registrarVenda(); 
                break;
            case 3: 
                cadastrarVendedor(); 
                break;
            case 4: 
                cadastrarProduto(); 
                break;
            case 5: 
                consultarVendasPorVendedor(); 
                break;
            case 6: 
                verTodosUsuarios(); 
                break;
            case 7: 
                editarUsuario(); 
                break;
            case 8: 
                editarProduto(); 
                break;
            case 9: 
                arquivarProduto(); 
                break;
            case 10: 
                arquivarUsuario(); 
                break;
            case 11: 
                cadastrarGerente(); 
                break;
            case 0:
                salvarTudo();
                printf("Saindo... Todos os dados foram salvos!\n");
                break;
            default:
                printf("Opção inválida! Digite um número da lista.\n");
        }

        // Pausa para a pessoa ler antes de limpar a tela
        if (opcaoEscolhida != 0) {
            printf("\nPressione ENTER para voltar ao menu...");
            while(getchar() != '\n');  // Limpa buffer
            getchar();  // Espera ENTER
        }
    }

    return 0;
}
