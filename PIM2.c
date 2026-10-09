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
    NivelAcesso nivel;     // Nível de permissão (1, 2 ou 3)
} Usuario;

// Armazena os dados de cada produto
typedef struct {
    char nome[50];         // Nome do produto
    float preco;           // Preço de uma unidade
    int quantidade;        // Quantidade disponível no estoque
} Produto;

// Armazena os dados de cada venda realizada
typedef struct {
    char vendedor[50];     // Nome de quem vendeu
    char produto[50];      // Nome do produto vendido
    int quantidade;        // Quantidade de unidades vendidas
    float valorTotal;      // Preço × quantidade
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
    if (!arquivo) {                             // Se não conseguiu abrir
        printf("ERRO: Não foi possível salvar usuários!\n");
        return;
    }
    // Percorre todos os usuários e grava cada um no arquivo
    for (int i = 0; i < qtdUsuarios; i++) {
        fprintf(arquivo, "%s;%s;%s;%d\n",
                usuarios[i].nome,
                usuarios[i].usuario,
                usuarios[i].senha,
                usuarios[i].nivel);
    }
    fclose(arquivo);  // Fecha o arquivo (importante para não perder dados)
}

// ==============================================
// FUNÇÃO: SALVAR PRODUTOS NO ARQUIVO
// Grava todos os produtos e estoque no arquivo produtos.txt
// ==============================================
void salvarProdutos() {
    FILE* arquivo = fopen(ARQ_PRODUTOS, "w");
    if (!arquivo) {
        printf("ERRO: Não foi possível salvar produtos!\n");
        return;
    }
    for (int i = 0; i < qtdProdutos; i++) {
        fprintf(arquivo, "%s;%.2f;%d\n",
                produtos[i].nome,
                produtos[i].preco,
                produtos[i].quantidade);
    }
    fclose(arquivo);
}

// ==============================================
// FUNÇÃO: SALVAR VENDAS NO ARQUIVO
// Grava todo o histórico de vendas no arquivo vendas.txt
// ==============================================
void salvarVendas() {
    FILE* arquivo = fopen(ARQ_VENDAS, "w");
    if (!arquivo) {
        printf("ERRO: Não foi possível salvar vendas!\n");
        return;
    }
    for (int i = 0; i < qtdVendas; i++) {
        fprintf(arquivo, "%s;%s;%d;%.2f\n",
                vendas[i].vendedor,
                vendas[i].produto,
                vendas[i].quantidade,
                vendas[i].valorTotal);
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
    if (!arquivo) {
        // Cria o administrador padrão
        strcpy(usuarios[0].nome, "Administrador Geral");
        strcpy(usuarios[0].usuario, "admin");
        strcpy(usuarios[0].senha, "123456");
        usuarios[0].nivel = ADMINISTRADOR;
        qtdUsuarios = 1;
        salvarUsuarios();  // Cria o arquivo com o admin padrão
        return;
    }

    // Lê cada linha do arquivo e coloca na lista de usuários
    while (fscanf(arquivo, "%[^;];%[^;];%[^;];%d\n",
                  usuarios[qtdUsuarios].nome,
                  usuarios[qtdUsuarios].usuario,
                  usuarios[qtdUsuarios].senha,
                  &usuarios[qtdUsuarios].nivel) == 4) {
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
    if (!arquivo) return;  // Se não existe, não tem o que carregar

    while (fscanf(arquivo, "%[^;];%f;%d\n",
                  produtos[qtdProdutos].nome,
                  &produtos[qtdProdutos].preco,
                  &produtos[qtdProdutos].quantidade) == 3) {
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
    if (!arquivo) return;

    while (fscanf(arquivo, "%[^;];%[^;];%d;%f\n",
                  vendas[qtdVendas].vendedor,
                  vendas[qtdVendas].produto,
                  &vendas[qtdVendas].quantidade,
                  &vendas[qtdVendas].valorTotal) == 4) {
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
    scanf("%s", user);
    printf("Digite sua senha: ");
    scanf("%s", senha);

    // Percorre a lista procurando usuário e senha iguais
    for (int i = 0; i < qtdUsuarios; i++) {
        if (strcmp(usuarios[i].usuario, user) == 0 &&
            strcmp(usuarios[i].senha, senha) == 0) {
            usuarioLogado = usuarios[i];  // Salva quem entrou
            printf("\nBem-vindo, %s!\n", usuarioLogado.nome);
            return 1;  // Login OK
        }
    }

    printf("\nUsuário ou senha incorretos!\n");
    return 0;  // Login falhou
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
            case ADMINISTRADOR: printf("Administrador\n"); break;
            case GERENTE:       printf("Gerente\n"); break;
            case VENDEDOR:      printf("Vendedor\n"); break;
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

    int numeroEscolhido;
    printf("\nDigite o NÚMERO do usuário que deseja editar: ");
    scanf("%d", &numeroEscolhido);
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

    printf("\n--- Editando: %s ---\n", usuarios[numeroEscolhido].nome);
    printf("→ Aperte ENTER sem digitar nada para MANTER o valor atual\n");

    char novoNome[50], novoUsuario[30], novaSenha[30];
    int novoNivel;

    // Nome
    printf("Novo nome [%s]: ", usuarios[numeroEscolhido].nome);
    scanf(" %[^\n]s", novoNome);
    if (strlen(novoNome) > 0)  // Se digitou algo → altera
        strcpy(usuarios[numeroEscolhido].nome, novoNome);

    // Usuário
    printf("Novo usuário [%s]: ", usuarios[numeroEscolhido].usuario);
    scanf("%s", novoUsuario);
    if (strlen(novoUsuario) > 0)
        strcpy(usuarios[numeroEscolhido].usuario, novoUsuario);

    // Senha
    printf("Nova senha: ");
    scanf("%s", novaSenha);
    if (strlen(novaSenha) > 0)
        strcpy(usuarios[numeroEscolhido].senha, novaSenha);

    // Nível de acesso → só Administrador pode mudar
    if (usuarioLogado.nivel == ADMINISTRADOR) {
        printf("Novo nível (1=Admin, 2=Gerente, 3=Vendedor) [%d]: ", usuarios[numeroEscolhido].nivel);
        scanf("%d", &novoNivel);
        if (novoNivel >= 1 && novoNivel <= 3)
            usuarios[numeroEscolhido].nivel = novoNivel;
    }

    salvarUsuarios();  // Salva as alterações no arquivo
    printf("✅ Usuário atualizado com sucesso!\n");
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

    Usuario novo;
    printf("\n--- Cadastro de Vendedor ---\n");
    printf("Nome completo: ");
    scanf(" %[^\n]s", novo.nome);
    printf("Usuário de acesso: ");
    scanf("%s", novo.usuario);
    printf("Senha: ");
    scanf("%s", novo.senha);
    novo.nivel = VENDEDOR;  // Define que é vendedor

    usuarios[qtdUsuarios] = novo;  // Adiciona na lista
    qtdUsuarios++;
    salvarUsuarios();  // Atualiza arquivo
    printf("✅ Vendedor cadastrado e salvo!\n");
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

    Usuario novo;
    printf("\n--- Cadastro de Gerente ---\n");
    printf("Nome completo: ");
    scanf(" %[^\n]s", novo.nome);
    printf("Usuário de acesso: ");
    scanf("%s", novo.usuario);
    printf("Senha: ");
    scanf("%s", novo.senha);
    novo.nivel = GERENTE;

    usuarios[qtdUsuarios] = novo;
    qtdUsuarios++;
    salvarUsuarios();
    printf("✅ Gerente cadastrado e salvo!\n");
}

// ==============================================
// FUNÇÃO: CADASTRAR PRODUTO
// Adiciona um produto novo no estoque
// Disponível para Gerente e Administrador
// ==============================================
void cadastrarProduto() {
    if (!temPermissao(GERENTE)) {
        printf("Acesso negado!\n");
        return;
    }

    Produto p;
    printf("\n--- Cadastro de Produto ---\n");
    printf("Nome do produto: ");
    scanf(" %[^\n]s", p.nome);
    printf("Preço unitário: R$ ");
    scanf("%f", &p.preco);
    printf("Quantidade em estoque: ");
    scanf("%d", &p.quantidade);

    produtos[qtdProdutos] = p;
    qtdProdutos++;
    salvarProdutos();
    printf("✅ Produto cadastrado e salvo!\n");
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
        printf("[%d] %-20s | Preço: R$ %8.2f | Estoque: %d unidades\n",
               i + 1,
               produtos[i].nome,
               produtos[i].preco,
               produtos[i].quantidade);
    }
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

    char nomeProduto[50];
    int quantidadeVendida;

    printf("\n--- Registrar Venda ---\n");
    printf("Nome do produto vendido: ");
    scanf(" %[^\n]s", nomeProduto);

    // Procura o produto pelo nome
    int indiceEncontrado = -1;
    for (int i = 0; i < qtdProdutos; i++) {
        if (strcmp(produtos[i].nome, nomeProduto) == 0) {
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

    // Verifica se tem estoque suficiente
    if (produtos[indiceEncontrado].quantidade < quantidadeVendida) {
        printf("Estoque insuficiente! Disponível: %d\n", produtos[indiceEncontrado].quantidade);
        return;
    }

    // Diminui do estoque
    produtos[indiceEncontrado].quantidade -= quantidadeVendida;

    // Cria o registro da venda
    Venda novaVenda;
    strcpy(novaVenda.vendedor, usuarioLogado.nome);
    strcpy(novaVenda.produto, produtos[indiceEncontrado].nome);
    novaVenda.quantidade = quantidadeVendida;
    novaVenda.valorTotal = quantidadeVendida * produtos[indiceEncontrado].preco;

    vendas[qtdVendas] = novaVenda;
    qtdVendas++;

    salvarProdutos();  // Atualiza estoque no arquivo
    salvarVendas();   // Salva a venda no arquivo

    printf("✅ Venda registrada! Valor total: R$ %.2f\n", novaVenda.valorTotal);
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
    scanf(" %[^\n]s", nomeVendedor);

    float totalGeral = 0;
    int encontrou = 0;

    printf("\n--- Vendas encontradas ---\n");
    for (int i = 0; i < qtdVendas; i++) {
        if (strcmp(vendas[i].vendedor, nomeVendedor) == 0) {
            printf("Produto: %-20s | Qtd: %3d | Valor: R$ %8.2f\n",
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
        printf("\n💰 TOTAL DE VENDAS: R$ %.2f\n", totalGeral);
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
        case ADMINISTRADOR: printf("ADMINISTRADOR\n"); break;
        case GERENTE:       printf("GERENTE\n"); break;
        case VENDEDOR:      printf("VENDEDOR\n"); break;
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
    }

    // Opção visível SÓ para Administrador
    if (usuarioLogado.nivel == ADMINISTRADOR) {
        printf("8  - Cadastrar Gerente\n");
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

    // Tenta logar → se falhar, fecha o programa
    if (!login()) {
        return 1;
    }

    int opcaoEscolhida;

    // ===== ESTRUTURA WHILE SIMPLIFICADA =====
    // Repete o menu enquanto a pessoa não digitar 0
    opcaoEscolhida = -1;  // Valor inicial qualquer que não seja 0
    while (opcaoEscolhida != 0) {
        exibirMenu();
        scanf("%d", &opcaoEscolhida);

        // Executa a função correspondente à opção
        switch (opcaoEscolhida) {
            case 1: consultarEstoque(); break;
            case 2: registrarVenda(); break;
            case 3: cadastrarVendedor(); break;
            case 4: cadastrarProduto(); break;
            case 5: consultarVendasPorVendedor(); break;
            case 6: verTodosUsuarios(); break;
            case 7: editarUsuario(); break;
            case 8: cadastrarGerente(); break;
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
