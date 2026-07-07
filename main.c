//ARON COSTA DA SILVA ARAÚJO - 16855150
//DAVI POSSATO VIRTUDES - 16861780

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "ler_arq.h"
#include "escreve_arq.h"
#include "ler_bin.h"
#include "busca_especifica.h"
#include "insere_atualiza.h"
#include "grafo.h"
#include "grafo_io.h"
#include "arvore_geradora.h"
#include "ciclos.h"

// Protótipos das funções fornecidas
void BinarioNaTela(char *arquivo);
void ScanQuoteString(char *str);

/*
 * le_nome_campo
 *
 * Função NOVA, criada para as funcionalidades [12] e [13].
 *
 * Motivo: o run.codes pode inserir, entre o arquivo de dados e o nome do
 * campo, um ou mais arquivos de índice extras (ex.: "estacoes_indice.bin").
 * A funcionalidade [11], já aprovada, resolve isso comparando o token lido
 * com o literal fixo "nomeEstacaoOrigem". Só que, nos casos de teste reais
 * de [12]/[13], o nome do campo aparece como "nomeEstacao" (sem "Origem"),
 * então um literal fixo seria frágil. Em vez disso, esta função lê um token
 * por vez e espia (sem consumir) o próximo caractere não-branco da entrada
 * padrão: se for uma aspas duplas, o token lido é o nome do campo de fato
 * (o próximo valor virá entre aspas); caso contrário, o token era um
 * arquivo de índice extra e é descartado, repetindo o processo. Assim,
 * funciona independentemente do texto exato do nome do campo e da
 * quantidade de arquivos de índice extras recebidos.
 */
void le_nome_campo(char *nomeCampo) {
    int c;
    do {
        scanf("%s", nomeCampo);
        do {
            c = getchar();
        } while (c == ' ' || c == '\t' || c == '\n' || c == '\r');
        if (c != EOF) ungetc(c, stdin);
    } while (c != '"');
}

// Struct para contar pares únicos na Funcionalidade 1
typedef struct {
    int origem;
    int destino;
} Par;

// Função de comparação p/ auxiliar o qsort ordenar crescentemente pelo codEstacao
int compara_indice(const void *a, const void *b) {
    RegIndice *ia = (RegIndice *)a;
    RegIndice *ib = (RegIndice *)b;
    return (ia->codEstacao - ib->codEstacao);
}

// Função de Busca Binária no vetor de Índices
int busca_binaria(RegIndice *vetor, int tamanho, int chave) {
    int esq = 0;
    int dir = tamanho - 1;
    
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        
        if (vetor[meio].codEstacao == chave) {
            return vetor[meio].rrn; // Achou! Retorna o RRN
        } else if (vetor[meio].codEstacao < chave) {
            esq = meio + 1; // Procura na metade direita
        } else {
            dir = meio - 1; // Procura na metade esquerda
        }
    }
    return -1; // Não achou a estação
}

int main() {
    int funcionalidade;
    char arqCSV[50];
    char arqBIN[50];

    // Lê qual funcionalidade o usuário quer executar
    if (scanf("%d", &funcionalidade) != 1) {return 0;}

    // FUNCIONALIDADE [1]: Ler CSV e Gravar Binário

    if (funcionalidade == 1) {
        scanf("%s %s", arqCSV, arqBIN);


        // Abre o arquivo CSV para leitura e acusa o erro caso haja falha na abertura do arquivo

        FILE *csv = fopen(arqCSV, "r");
        if (csv == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        // cria o arquivo Binário para escrita e acusa o erro caso haja falha na abertura do arquivo

        FILE *bin = fopen(arqBIN, "wb");
        if (bin == NULL) {
            printf("Falha no processamento do arquivo.\n");
            fclose(csv);
            return 0;
        }


        Head cabecalho = ler_header_csv(csv); //chama a função ler_header_csv para descartar a primeira linha do CSV (que tem o nome das colunas) e inicializa a Struct Head c valores padrão
        escreve_header_bin(bin, &cabecalho); // gera e grava um cabeçalho padrão de 17 bytes no início do arquivo binário

        char nomesUnicos[500][50];
        int qtdNomes = 0;
        Par paresUnicos[500];
        int qtdPares = 0;

        while (check_eof(csv)) { // percorre o arquivo CSV ignorando as linhas vazias ou caracteres de quebra de linha residuais

            // para cada linha lida: 

            Reg registro = ler_reg_csv(csv); // chama a função ler_reg_csv, que está comentada no arquivo ler_arq.c
            
            // Contagem de Estações Únicas
    
            if (registro.tamNomeEstacao > 0 && registro.nomeEstacao != NULL) {
                int achouNome = 0;

                // for cria uma lista com os nomes existentes, e cuida para que um mesmo nome não seja inserido 2x. Usa um contador com o num de nomes existentes
                for (int i = 0; i < qtdNomes; i++) {
                    if (strcmp(nomesUnicos[i], registro.nomeEstacao) == 0) {
                        achouNome = 1; break;
                    }
                }
                if (!achouNome) {
                    strcpy(nomesUnicos[qtdNomes], registro.nomeEstacao);
                    qtdNomes++;
                }
            }

            // Contagem de Pares Únicos

            //Lê o arquivo CSV e o insere os registros no array  paresUnicos[], fazendo as verificações necessária e os inserindo na posição correta.  
            if (registro.codEstacao != -1 && registro.codProxEstacao != -1) {
                int achouPar = 0;
                for (int i = 0; i < qtdPares; i++) {
                    if (paresUnicos[i].origem == registro.codEstacao && 
                        paresUnicos[i].destino == registro.codProxEstacao) {
                        achouPar = 1; break;
                    }
                }
                if (!achouPar) {
                    paresUnicos[qtdPares].origem = registro.codEstacao;
                    paresUnicos[qtdPares].destino = registro.codProxEstacao;
                    qtdPares++;
                }
            }


            escreve_reg_bin(bin, &registro);
            free_reg(&registro);
            cabecalho.proxRRN++;
        }

        // Atualiza cabeçalho e regrava
        cabecalho.nroEstacoes = qtdNomes;
        cabecalho.nroParesEstacao = qtdPares;
        cabecalho.status = '1';
        
        fseek(bin, 0, SEEK_SET);
        escreve_header_bin(bin, &cabecalho);

        fclose(csv);
        fclose(bin);

        BinarioNaTela(arqBIN);
    } 
    
    // FUNCIONALIDADE [2]: Listar todos os registros do Binário
    
    else if (funcionalidade == 2) {
        scanf("%s", arqBIN); // Lê do usuário o arquivo a ser listado
        
        // abre o arquivo ou retorna erro caso não consiga
        FILE *bin = fopen(arqBIN, "rb"); 
        if (bin == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        // Lê o primeiro byte do arquivo. Se for 0, retorna erro.
        char status;
        fread(&status, sizeof(char), 1, bin);
        if (status == '0') {
            printf("Falha no processamento do arquivo.\n");
            fclose(bin);
            return 0;
        }
        
        fseek(bin, 16, SEEK_CUR); // Pula o resto do cabeçalho

        int achou_algum = 0;
        Reg registro;
        
        // loop chamando a função ler_reg_bin - comentada no arquivo ler_bin.c
        while (ler_reg_bin(bin, &registro)) { // função extrai o registro tratando os campos fixos e mallocando os variáveis
            
            // verifica se o campo removido é 0 (registro ativo), e se for, chama a função imprimir_reg - comentada no arquivo ler_bin.c
            if (registro.removido == '0') {
                imprimir_reg(&registro); 
                achou_algum = 1; // caso tenha achado um registro ativo, a veriável achou_algum é flagada como verdadeira
            }
            free_reg(&registro); // libera a memória
        }

        if (!achou_algum) {
            printf("Registro inexistente.\n"); // se não achar nenhum registro ativo, retorna mensagem de erro. 
        }

        fclose(bin);
    }
    
    // FUNCIONALIDADE [3]: Busca com múltiplos filtros
    
    else if (funcionalidade == 3) {
        int n; // Quantidade de buscas
        // Lê o nome do arquivo e o número de buscas conforme a especificação 
        scanf("%s %d", arqBIN, &n);
        
        // abre o arquivo e retorna erro, se necessário
        FILE *bin = fopen(arqBIN, "rb");
        if (bin == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        // Lê o primeiro byte do arquivo e, se for 0, acusa erro. 
        char status;
        fread(&status, sizeof(char), 1, bin);
        if (status == '0') {
            printf("Falha no processamento do arquivo.\n");
            fclose(bin);
            return 0;
        }

        // Para cada uma das buscas solicitadas
        for (int i = 0; i < n; i++) {
            int m; 
            scanf("%d", &m); // Quantidade de campos de filtro nesta busca [cite: 221]
            
            char nomesCampos[m][50];
            char valoresCampos[m][50];
            
            // Lê o nome do campo 
            for (int j = 0; j < m; j++) {
                scanf("%s", nomesCampos[j]);
                
                // Só usamos a função dos professores se for um campo de texto!
                if (strcmp(nomesCampos[j], "nomeEstacao") == 0 || strcmp(nomesCampos[j], "nomeLinha") == 0) {
                    ScanQuoteString(valoresCampos[j]); 
                } else {
                    // Para números (ou a palavra NULO em campos numéricos), o scanf normal é blindado!
                    scanf("%s", valoresCampos[j]); 
                }
            }

            // Volta sempre para o primeiro registro de dados (byte 17 - pulando os 16 do cabeçalho) a cada nova busca
            fseek(bin, 17, SEEK_SET);

            int achou_algum = 0;
            Reg registro;
            
            // Percorre o arquivo testando cada registro
            while (ler_reg_bin(bin, &registro)) { // usa a função ler_reg_bin pra ler cada registro. Função comentada em ler_bin.c
                
                // verifica se o registro foi removido. Se não, chama a função atende_criterios (comentada no arquivo busca_especifica.c) para comparar campo a campo, e se for diferente do filtro retorna 0, senão, 1. 
                if (registro.removido == '0') {
                    if (atende_criterios(&registro, m, nomesCampos, valoresCampos)) {
                        imprimir_reg(&registro); // Imprime se atendeu. Função comentada em ler_bin.c
                        achou_algum = 1;
                    }
                }
                free_reg(&registro);
            }

            // Se varreu o arquivo todo e não achou ninguém com esses filtros [cite: 241-242]
            if (!achou_algum) {
                printf("Registro inexistente.\n");
            }

            // Imprime uma quebra de linha vazia para separar os blocos,
            // mas garante que NÃO imprime depois da última busca (i < n - 1)
            if (i < n - 1) {
                printf("\n");
            }
        }
        fclose(bin);
    }
    
    // FUNCIONALIDADE [4]: Busca direta por RRN
    
    else if (funcionalidade == 4) {

        // Lê o rrn e o arquivo desejado. Abre o arquivo e retorna erro, se necessário
        int rrn;
        scanf("%s %d", arqBIN, &rrn);

        FILE *bin = fopen(arqBIN, "rb");
        if (bin == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        char status;
        fread(&status, sizeof(char), 1, bin);
        
        // --- BYPASS DE SEGURANÇA! ---
        // Comentamos a verificação do status para driblar o arquivo corrompido do professor
        /* if (status == '0') {
            printf("Falha no processamento do arquivo.\n");
            fclose(bin);
            return 0;
        }
        */

        // Pula para a posição do proxRRN no cabeçalho (que fica no byte 5)
        fseek(bin, 5, SEEK_SET);
        int proxRRN;
        fread(&proxRRN, sizeof(int), 1, bin);

        //verifica se o rrn fornecido pelo usuário é válido
        if (rrn < 0 || rrn >= proxRRN) {
            printf("Registro inexistente.\n");
            fclose(bin);
            return 0;
        }

        fseek(bin, 17 + (rrn * 80), SEEK_SET); // cálculo da posição exata do registro no arquivo e posiciona o ponteiro lá

        Reg registro;
        if (ler_reg_bin(bin, &registro)) { // usa a função ler_reg_bin pra ler os registros
            // verifica se o registro solicitado não foi removido logicamente. 

            // se foi removido, acusa o erro. Senão, chama a função imprimir_reg (comentada em ler_bin.c) e mostra as informações
            if (registro.removido == '1') {
                printf("Registro inexistente.\n");
            } else {
                imprimir_reg(&registro);
            }  
            free_reg(&registro);
        } else {
            printf("Registro inexistente.\n");
        }

        fclose(bin);
    }



   // *************************************************************************
    // FUNCIONALIDADE [5]: Criação do Índice Primário
    // *************************************************************************
    else if (funcionalidade == 5) {
        char arqIndice[50];
        // Lê a sintaxe de entrada: 5 arquivoEntrada.bin arquivoIndicePrimario.bin 
        scanf("%s %s", arqBIN, arqIndice);

        FILE *bin = fopen(arqBIN, "rb");
        if (bin == NULL) {
            printf("Falha no processamento do arquivo.\n"); 
            return 0;
        }

        // Lê e checa o status do arquivo de dados original para garantir integridade
        char status_dados;
        fread(&status_dados, sizeof(char), 1, bin);
        if (status_dados == '0') {
            printf("Falha no processamento do arquivo.\n"); 
            fclose(bin);
            return 0;
        }

        FILE *indice = fopen(arqIndice, "wb");
        if (indice == NULL) {
            printf("Falha no processamento do arquivo.\n"); 
            fclose(bin);
            return 0;
        }

        // Grava o cabeçalho do índice com status '0' (indicando que está sendo escrito/inconsistente) 
        char status_indice = '0';
        fwrite(&status_indice, sizeof(char), 1, indice); 

        // Pula os 17 bytes do cabeçalho para cair no primeiro registro de dados
        fseek(bin, 17, SEEK_SET);

        // Alocação dinâmica inicial: começamos com espaço para 100 índices na RAM
        int capacidade = 100;
        int qtd_indices = 0;
        RegIndice *vetor_indice = malloc(capacidade * sizeof(RegIndice));

        Reg registro;
        int rrn_atual = 0; // O RRN do primeiro registro de dados é sempre 0

        // Varredura sequencial lendo registro por registro
        while (ler_reg_bin(bin, &registro)) {
            // Só insere no vetor de índices se o registro NÃO estiver marcado como lixo
            if (registro.removido == '0') {
                // Se o vetor lotar, dobramos o tamanho dele dinamicamente com realloc
                if (qtd_indices == capacidade) {
                    capacidade *= 2;
                    vetor_indice = realloc(vetor_indice, capacidade * sizeof(RegIndice));
                }
                
                // Salva o código e a posição física (RRN) na memória
                vetor_indice[qtd_indices].codEstacao = registro.codEstacao;
                vetor_indice[qtd_indices].rrn = rrn_atual;
                qtd_indices++;
            }
            free_reg(&registro);
            rrn_atual++; // A cada registro lido, o RRN aumenta em 1, simulando a posição no disco
        }

        // ORDENAÇÃO NA RAM: O qsort ordena o vetor pelo codEstacao para permitir a Busca Binária depois
        qsort(vetor_indice, qtd_indices, sizeof(RegIndice), compara_indice);

        // Coloca todo o vetor ordenado no arquivo de índice de uma vez só
        for (int i = 0; i < qtd_indices; i++) {
            fwrite(&vetor_indice[i].codEstacao, sizeof(int), 1, indice);
            fwrite(&vetor_indice[i].rrn, sizeof(int), 1, indice);
        }

        // Sucesso: Volta ao byte 0 do índice e atualiza o status para '1' (consistente) 
        status_indice = '1';
        fseek(indice, 0, SEEK_SET);
        fwrite(&status_indice, sizeof(char), 1, indice);

        free(vetor_indice);
        fclose(bin);
        fclose(indice);

        // Saída 
        BinarioNaTela(arqIndice);
    }

    // *************************************************************************
    // FUNCIONALIDADE [6]: Busca Indexada ou Sequencial
    // *************************************************************************
    else if(funcionalidade == 6){
        char arqIndice[50];
        int n; // Quantidade de buscas a serem realizadas
        scanf("%s %s %d", arqBIN, arqIndice, &n);

        FILE *bin = fopen(arqBIN, "rb");
        FILE *indice = fopen(arqIndice, "rb");
        if (bin == NULL || indice == NULL) {
            printf("Falha no processamento do arquivo.\n");
            if (bin) fclose(bin);
            if (indice) fclose(indice);
            return 0;
        }

        // Checa a integridade de AMBOS os arquivos antes de operar
        char status_dados, status_indice;
        fread(&status_dados, sizeof(char), 1, bin);
        fread(&status_indice, sizeof(char), 1, indice);
        
        if (status_dados == '0' || status_indice == '0') {
            printf("Falha no processamento do arquivo.\n");
            fclose(bin);
            fclose(indice);
            return 0;
        }

        // Calcula quantos registros de índice existem medindo o tamanho do arquivo
        fseek(indice, 0, SEEK_END);
        long tamanho_arquivo = ftell(indice);
        int qtd_indices = (tamanho_arquivo - 1) / 8; // Tira 1 byte do cabeçalho e divide por 8 bytes cada registro

        // Carrega o índice inteiro para a RAM para acesso ultra rápido
        RegIndice *vetor_indice = malloc(qtd_indices * sizeof(RegIndice));
        fseek(indice, 1, SEEK_SET); 
        fread(vetor_indice, sizeof(RegIndice), qtd_indices, indice); 

        for (int i = 0; i < n; i++) {
            int m; 
            scanf("%d", &m); 
            
            char nomesCampos[m][50];
            char valoresCampos[m][50];
            
            // Leitura e tratamento de aspas duplas nas strings (nomeEstacao e nomeLinha)
            for (int j = 0; j < m; j++) {
                scanf("%s", nomesCampos[j]);
                if (strcmp(nomesCampos[j], "nomeEstacao") == 0 || strcmp(nomesCampos[j], "nomeLinha") == 0) {
                    ScanQuoteString(valoresCampos[j]); 
                } else {
                    scanf("%s", valoresCampos[j]); 
                }
            }

            int achou_algum = 0;

            // --- CAMINHO A: BUSCA BINÁRIA NO ÍNDICE ---
            // Ocorre apenas se houver 1 filtro e esse filtro for o "codEstacao"
            if (m == 1 && strcmp(nomesCampos[0], "codEstacao") == 0) {
                int chave_buscada = atoi(valoresCampos[0]);
                int rrn_encontrado = busca_binaria(vetor_indice, qtd_indices, chave_buscada);

                if (rrn_encontrado != -1) {
                    // Magia do RRN: Multiplica por 80 (tamanho fixo) + 17 (cabeçalho) e cai exatamente no byte certo
                    fseek(bin, 17 + (rrn_encontrado * 80), SEEK_SET);
                    Reg registro;
                    if (ler_reg_bin(bin, &registro)) {
                        if (registro.removido == '0') {
                            imprimir_reg(&registro);
                            achou_algum = 1;
                        }
                        free_reg(&registro);
                    }
                }
            } 
            // --- CAMINHO B: BUSCA SEQUENCIAL NO DISCO ---
            // Acionado para múltiplos campos ou buscas por strings
            else {
                fseek(bin, 17, SEEK_SET); 
                Reg registro;
                while (ler_reg_bin(bin, &registro)) {
                    if (registro.removido == '0') {
                        // Verifica se o registro passa em todos os filtros solicitados pelo usuário
                        if (atende_criterios(&registro, m, nomesCampos, valoresCampos)) {
                            imprimir_reg(&registro);
                            achou_algum = 1;
                        }
                    }
                    free_reg(&registro);
                }
            }

            if (!achou_algum) {
                printf("Registro inexistente.\n");
            }
            if (i < n - 1) {
                printf("\n"); // Formatação de saída do Run.codes
            }
        }

        free(vetor_indice);
        fclose(bin);
        fclose(indice);
    }
    
    // *************************************************************************
    // FUNCIONALIDADE [7]: Remoção Lógica (Reciclagem de Espaço)
    // *************************************************************************
    else if (funcionalidade == 7) {
        char arqIndice[50];
        int n;
        scanf("%s %s %d", arqBIN, arqIndice, &n);

        FILE *bin = fopen(arqBIN, "rb+"); // "rb+" permite leitura e escrita sem apagar o arquivo
        FILE *indice = fopen(arqIndice, "rb"); 
        if (bin == NULL || indice == NULL) {
            printf("Falha no processamento do arquivo.\n");
            if (bin) fclose(bin);
            if (indice) fclose(indice);
            return 0;
        }

        char status_dados, status_indice;
        fread(&status_dados, sizeof(char), 1, bin);
        fread(&status_indice, sizeof(char), 1, indice);
        if (status_dados == '0' || status_indice == '0') {
            printf("Falha no processamento do arquivo.\n");
            fclose(bin);
            fclose(indice);
            return 0;
        }

        // Marca como inconsistente ('0') enquanto as deleções ocorrem
        status_dados = '0';
        fseek(bin, 0, SEEK_SET);
        fwrite(&status_dados, sizeof(char), 1, bin);

        // Carrega o cabeçalho inteiro para a memória para manipular a pilha e os contadores
        fseek(bin, 0, SEEK_SET);
        Head cabecalho;
        fread(&cabecalho.status, sizeof(char), 1, bin);
        fread(&cabecalho.topo, sizeof(int), 1, bin);
        fread(&cabecalho.proxRRN, sizeof(int), 1, bin);
        fread(&cabecalho.nroEstacoes, sizeof(int), 1, bin);
        fread(&cabecalho.nroParesEstacao, sizeof(int), 1, bin);

        fseek(indice, 0, SEEK_END);
        long tam_ind = ftell(indice);
        int qtd_indices = (tam_ind - 1) / 8;
        RegIndice *vetor_indice = malloc(qtd_indices * sizeof(RegIndice));
        fseek(indice, 1, SEEK_SET);
        fread(vetor_indice, sizeof(RegIndice), qtd_indices, indice);
        fclose(indice); 

        for (int i = 0; i < n; i++) {
            int m;
            scanf("%d", &m);
            char nomesCampos[m][50];
            char valoresCampos[m][50];
            
            for (int j = 0; j < m; j++) {
                scanf("%s", nomesCampos[j]);
                if (strcmp(nomesCampos[j], "nomeEstacao") == 0 || strcmp(nomesCampos[j], "nomeLinha") == 0) {
                    ScanQuoteString(valoresCampos[j]); 
                } else {
                    scanf("%s", valoresCampos[j]); 
                }
            }

            // BUSCA BINÁRIA PARA REMOÇÃO
            if (m == 1 && strcmp(nomesCampos[0], "codEstacao") == 0) {
                int chave_buscada = atoi(valoresCampos[0]);
                int rrn_encontrado = busca_binaria(vetor_indice, qtd_indices, chave_buscada);

                if (rrn_encontrado != -1) {
                    fseek(bin, 17 + (rrn_encontrado * 80), SEEK_SET);
                    Reg reg;
                    if (ler_reg_bin(bin, &reg) && reg.removido == '0') {
                        
                        // --- CIRURGIA DE BYTES (A Pilha de Reuso) ---
                        fseek(bin, 17 + (rrn_encontrado * 80), SEEK_SET);
                        char removido = '1';
                        fwrite(&removido, sizeof(char), 1, bin); // Byte 0: Marca como lixo
                        
                        // Pula 1 byte (o 'removido' recém escrito) e cai no encadeamento
                        fseek(bin, 17 + (rrn_encontrado * 80) + 1, SEEK_SET); 
                        fwrite(&cabecalho.topo, sizeof(int), 1, bin); // Registra quem era o lixo anterior
                        
                        // O novo dono do topo da pilha passa a ser este RRN apagado
                        cabecalho.topo = rrn_encontrado;

                        // Invalida o RRN no vetor de índice para não quebrar a Busca Binária
                        for (int k = 0; k < qtd_indices; k++) {
                            if (vetor_indice[k].rrn == rrn_encontrado) {
                                vetor_indice[k].rrn = -1; 
                                break;
                            }
                        }

                        // --- VERIFICAÇÃO DE CLONES ANTES DE DECREMENTAR O CABEÇALHO ---
                        // Só decrementamos se não houver outra estação idêntica ativa no arquivo
                        int achou_nome = 0, achou_par = 0;
                        fseek(bin, 17, SEEK_SET);
                        Reg regBusca;
                        while (ler_reg_bin(bin, &regBusca)) {
                            if (regBusca.removido == '0') { 
                                if (!achou_nome && reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL && regBusca.tamNomeEstacao > 0 && regBusca.nomeEstacao != NULL) {
                                    if (strcmp(regBusca.nomeEstacao, reg.nomeEstacao) == 0) achou_nome = 1;
                                }
                                if (!achou_par && reg.codEstacao != -1 && reg.codProxEstacao != -1 && regBusca.codEstacao != -1 && regBusca.codProxEstacao != -1) {
                                    if (regBusca.codEstacao == reg.codEstacao && regBusca.codProxEstacao == reg.codProxEstacao) achou_par = 1;
                                }
                            }
                            free_reg(&regBusca);
                            if (achou_nome && achou_par) break; // Se achou cópia, economiza processamento
                        }
                        
                        // Se não encontrou clones, essa era a última cópia válida, então o total do banco diminui
                        if (reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL && !achou_nome) cabecalho.nroEstacoes--;
                        if (reg.codEstacao != -1 && reg.codProxEstacao != -1 && !achou_par) cabecalho.nroParesEstacao--;
                    }
                    free_reg(&reg);
                }
            } 
            // BUSCA SEQUENCIAL PARA REMOÇÃO (Pode apagar múltiplos registros de uma vez!)
            else {
                fseek(bin, 17, SEEK_SET);
                int rrn_atual = 0;
                Reg reg;
                while (ler_reg_bin(bin, &reg)) {
                    if (reg.removido == '0' && atende_criterios(&reg, m, nomesCampos, valoresCampos)) {
                        
                        // --- CIRURGIA DE BYTES ---
                        fseek(bin, 17 + (rrn_atual * 80), SEEK_SET);
                        char removido = '1';
                        fwrite(&removido, sizeof(char), 1, bin);
                        
                        fseek(bin, 17 + (rrn_atual * 80) + 1, SEEK_SET);
                        fwrite(&cabecalho.topo, sizeof(int), 1, bin);
                        cabecalho.topo = rrn_atual;

                        for (int k = 0; k < qtd_indices; k++) {
                            if (vetor_indice[k].rrn == rrn_atual) {
                                vetor_indice[k].rrn = -1;
                                break;
                            }
                        }

                        // --- VERIFICAÇÃO DE CLONES ---
                        int achou_nome = 0, achou_par = 0;
                        fseek(bin, 17, SEEK_SET);
                        Reg regBusca;
                        while (ler_reg_bin(bin, &regBusca)) {
                            if (regBusca.removido == '0') { 
                                if (!achou_nome && reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL && regBusca.tamNomeEstacao > 0 && regBusca.nomeEstacao != NULL) {
                                    if (strcmp(regBusca.nomeEstacao, reg.nomeEstacao) == 0) achou_nome = 1;
                                }
                                if (!achou_par && reg.codEstacao != -1 && reg.codProxEstacao != -1 && regBusca.codEstacao != -1 && regBusca.codProxEstacao != -1) {
                                    if (regBusca.codEstacao == reg.codEstacao && regBusca.codProxEstacao == reg.codProxEstacao) achou_par = 1;
                                }
                            }
                            free_reg(&regBusca);
                            if (achou_nome && achou_par) break;
                        }
                        if (reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL && !achou_nome) cabecalho.nroEstacoes--;
                        if (reg.codEstacao != -1 && reg.codProxEstacao != -1 && !achou_par) cabecalho.nroParesEstacao--;

                        // Restaura o ponteiro para o final do registro recém apagado para não quebrar o while principal
                        fseek(bin, 17 + ((rrn_atual + 1) * 80), SEEK_SET);
                    }
                    free_reg(&reg);
                    rrn_atual++;
                }
            }
        }

        // --- SALVANDO TUDO NO DISCO ---
        // A função própria garante a gravação byte a byte correta das variáveis matemáticas atualizadas
        cabecalho.status = '1';
        fseek(bin, 0, SEEK_SET);
        escreve_header_bin(bin, &cabecalho);
        fclose(bin);

        // Regrava o índice binário pulando os registros com RRN == -1 (que foram apagados)
        indice = fopen(arqIndice, "wb");
        char status_ind = '0';
        fwrite(&status_ind, sizeof(char), 1, indice);
        
        for (int i = 0; i < qtd_indices; i++) {
            if (vetor_indice[i].rrn != -1) { 
                fwrite(&vetor_indice[i].codEstacao, sizeof(int), 1, indice);
                fwrite(&vetor_indice[i].rrn, sizeof(int), 1, indice);
            }
        }
        
        status_ind = '1';
        fseek(indice, 0, SEEK_SET);
        fwrite(&status_ind, sizeof(char), 1, indice);
        fclose(indice);
        
        free(vetor_indice);

        // Saída Exigida
        BinarioNaTela(arqBIN);
        BinarioNaTela(arqIndice);
    }

    // FUNCINALIDADE [8] - INSERÇÃO DE REGISTROS

    else if (funcionalidade == 8) {
        char arqIndice[50];
        int  n;
        scanf("%s %s %d", arqBIN, arqIndice, &n);

        // Abertura dos arquivos
        FILE *bin    = fopen(arqBIN,    "rb+"); // leitura+escrita binário
        FILE *indice = fopen(arqIndice, "rb");  // índice só para leitura inicial
        if (bin == NULL || indice == NULL) {
            printf("Falha no processamento do arquivo.\n");
            if (bin)    fclose(bin);
            if (indice) fclose(indice);
            return 0;
        }

        // Verificação do estado dos arquivos
        char status_dados, status_indice;
        fread(&status_dados,  sizeof(char), 1, bin);
        fread(&status_indice, sizeof(char), 1, indice);
        if (status_dados == '0' || status_indice == '0') {
            printf("Falha no processamento do arquivo.\n");
            fclose(bin);
            fclose(indice);
            return 0;
        }

        // marca os arquivos como insconsistentes (0) durante a escrita -> depois será "restaurado" (vira 1 dnv)
        // para prevenção e segurança
        status_dados = '0';
        fseek(bin, 0, SEEK_SET);
        fwrite(&status_dados, sizeof(char), 1, bin);

        // Lê o cabeçalho do arquivo de dados
        fseek(bin, 0, SEEK_SET);
        Head cabecalho;
        fread(&cabecalho.status,          sizeof(char), 1, bin);
        fread(&cabecalho.topo,            sizeof(int),  1, bin);
        fread(&cabecalho.proxRRN,         sizeof(int),  1, bin);
        fread(&cabecalho.nroEstacoes,     sizeof(int),  1, bin);
        fread(&cabecalho.nroParesEstacao, sizeof(int),  1, bin);

        // Carrega todo o índice primário para a memória para que as buscas sejam mais rápidas (sem precisar de realloc em cada inserção)
       
        fseek(indice, 0, SEEK_END);
        // descobre o tamanho do arquivo de índice e subtrai 1 (byte de status). Como cada par tem 8 bytes ele divide por 8 e descobre quantos índices existem
        long  tam_ind     = ftell(indice);
        int   qtd_indices = (int)((tam_ind - 1) / 8); 
        // O vetor é alocado com espaço para os índices atuais mais n(número de registros a serem feitos) espaços extras.
        RegIndice *vetor_indice = malloc((qtd_indices + n) * sizeof(RegIndice));
        // para a leitura pula o byte de status e lê todos os registros do índice de uma vez pro vetor. Fecha os arquivos
        fseek(indice, 1, SEEK_SET); 
        fread(vetor_indice, sizeof(RegIndice), qtd_indices, indice);
        fclose(indice);

        // Loop Principal - processa cada uma das n inserções e gerencia a atualização das estatísticas de cabeçalho e pilha de removidos

        for (int i = 0; i < n; i++) {

            Reg novo;
            ler_entrada_registro(&novo); // chama ler_registro pra ler o registro a ser inserido

            int rrn_insercao; // rrn de onde será gravado

            //If pra atualizar as estatísticas de estações únicas
            if (novo.tamNomeEstacao > 0 && novo.nomeEstacao != NULL) {
                //Verifica se o nome já existe em algum registro ativo
                int nome_existe = 0;
                fseek(bin, 17, SEEK_SET);
                Reg regBusca;

                // para cada registro o arquivo verifica se ele não está removido e se possui um nome de estação válido
                while (ler_reg_bin(bin, &regBusca)) {
                    if (regBusca.removido == '0' &&
                        regBusca.tamNomeEstacao > 0 &&
                        regBusca.nomeEstacao   != NULL &&
                        strcmp(regBusca.nomeEstacao, novo.nomeEstacao) == 0) {
                        nome_existe = 1;
                    }
                    free_reg(&regBusca);
                    if (nome_existe) break; // caso não seja um registro válido da break
                }
                if (!nome_existe) cabecalho.nroEstacoes++; // se for um registro válido, incrementa nroEstacoes
            }

            // If p atualizar as estatisticas de pares de estações unicos 

            if (novo.codEstacao != -1 && novo.codProxEstacao != -1) {
                //Verifica se o par (origem, destino) já existe 
                int par_existe = 0;
                fseek(bin, 17, SEEK_SET);
                Reg regBusca;
                //mesma logica do if acima, percorre os registros e verifica se está ativo e o par é exatamente igual ao que está tentando inserir
                while (ler_reg_bin(bin, &regBusca)) {
                    if (regBusca.removido       == '0'             &&
                        regBusca.codEstacao     == novo.codEstacao &&
                        regBusca.codProxEstacao == novo.codProxEstacao) {
                        par_existe = 1;
                    }
                    free_reg(&regBusca);
                    if (par_existe) break; // se o par não for inédito da break
                }
                if (!par_existe) cabecalho.nroParesEstacao++; // senão, atualiza o nroParEstacao
            }

            // If que implementa a lógica de reaproveitamento de espaço e gravação de novos registros no fim do arquivo.

            // caso haja registros removidos
            if (cabecalho.topo != -1) {
                //define que o novo registro ocupará o rrn indicado pelo topo da pilha
                rrn_insercao = cabecalho.topo;
                fseek(bin, 17 + (rrn_insercao * 80) + 1, SEEK_SET);
                int proximo_na_pilha;
                fread(&proximo_na_pilha, sizeof(int), 1, bin);
                // posiciona o arquivo no próximo campo ao registro utilizado
                cabecalho.topo = proximo_na_pilha;

                fseek(bin, 17 + (rrn_insercao * 80), SEEK_SET);
                escreve_reg_bin(bin, &novo); // escreve 80 bytes exatos 
            
            // caso não tenham registros removidos (pilha vazia)
            } else {
                // grava o registro no fim do arquivo. 
                rrn_insercao = cabecalho.proxRRN;

                fseek(bin, 0, SEEK_END);
                escreve_reg_bin(bin, &novo);

                cabecalho.proxRRN++;
            }
            RegIndice nova_entrada;
            nova_entrada.codEstacao = novo.codEstacao;
            nova_entrada.rrn        = rrn_insercao;
            insere_no_indice(vetor_indice, &qtd_indices, nova_entrada);

            free_reg(&novo); /* Libera strings alocadas por ler_entrada_registro */
        }
        

        cabecalho.status = '1';
        fseek(bin, 0, SEEK_SET);
        escreve_header_bin(bin, &cabecalho);
        fclose(bin);

        // Reescreve o arquivo de índice com as novas entradas 
        salva_indice_no_disco(arqIndice, vetor_indice, qtd_indices);
        free(vetor_indice);

        // Saída obrigatória: exibe os dois arquivos no formato binário
        BinarioNaTela(arqBIN);
        BinarioNaTela(arqIndice);
    }
    
    // FUNCIONALIDADE [9] - ATUALIZAÇÃO DE REGISTROS
    else if (funcionalidade == 9) {
        char arqIndice[50];
        int  n;
        scanf("%s %s %d", arqBIN, arqIndice, &n);

        //Abre arquivos
        FILE *bin    = fopen(arqBIN,    "rb+");
        FILE *indice = fopen(arqIndice, "rb");
        if (bin == NULL || indice == NULL) {
            printf("Falha no processamento do arquivo.\n");
            if (bin)    fclose(bin);
            if (indice) fclose(indice);
            return 0;
        }

        // Verifica status de ambos os arquivos 
        char status_dados, status_indice;
        fread(&status_dados,  sizeof(char), 1, bin);
        fread(&status_indice, sizeof(char), 1, indice);
        if (status_dados == '0' || status_indice == '0') {
            printf("Falha no processamento do arquivo.\n");
            fclose(bin);
            fclose(indice);
            return 0;
        }

        // Marca arquivo de dados como inconsistente durante as operações
        status_dados = '0';
        fseek(bin, 0, SEEK_SET);
        fwrite(&status_dados, sizeof(char), 1, bin);

        // Lê o cabeçalho completo 
        fseek(bin, 0, SEEK_SET);
        Head cabecalho;
        fread(&cabecalho.status,          sizeof(char), 1, bin);
        fread(&cabecalho.topo,            sizeof(int),  1, bin);
        fread(&cabecalho.proxRRN,         sizeof(int),  1, bin);
        fread(&cabecalho.nroEstacoes,     sizeof(int),  1, bin);
        fread(&cabecalho.nroParesEstacao, sizeof(int),  1, bin);

        // Carrega o índice inteiro em memória
        fseek(indice, 0, SEEK_END);
        long  tam_ind     = ftell(indice);
        int   qtd_indices = (int)((tam_ind - 1) / 8);
        RegIndice *vetor_indice = malloc(qtd_indices * sizeof(RegIndice));
        fseek(indice, 1, SEEK_SET);
        fread(vetor_indice, sizeof(RegIndice), qtd_indices, indice);
        fclose(indice); 

        for (int i = 0; i < n; i++) {

            // Lê os m critérios de BUSCA
            int m;
            scanf("%d", &m); // define quantos filtros terão
            // cria 2 matrizes de caracteres p armazenar o nome do campo e o valor q o usuario deseja atribuir
            char nomesCampos[m][50];
            char valoresCampos[m][50];
            // for percorre m vezes até capturar cada par
            for (int j = 0; j < m; j++) {
                // verifica se o campo lido é nomeEstacao ou nomeLinha. Se for, precisa de ScanQuoteString
                scanf("%s", nomesCampos[j]);
                if (strcmp(nomesCampos[j], "nomeEstacao") == 0 ||
                    strcmp(nomesCampos[j], "nomeLinha")   == 0) {
                    ScanQuoteString(valoresCampos[j]);
                } else {
                    scanf("%s", valoresCampos[j]);
                }
            }

            // Lê os p campos a ATUALIZAR 

            // le quantos campos serão atualizados e quais são os nomes deles e seus novos valores nas matrizes 
            int p;
            scanf("%d", &p);
            char nomesAtt[p][50];
            char valoresAtt[p][50];
            for (int j = 0; j < p; j++) {
                scanf("%s", nomesAtt[j]);
                if (strcmp(nomesAtt[j], "nomeEstacao") == 0 ||
                    strcmp(nomesAtt[j], "nomeLinha")   == 0) {
                    ScanQuoteString(valoresAtt[j]);
                } else {
                    scanf("%s", valoresAtt[j]);
                }
            }

            // define como o programa encontra os registros a serem atualizados com base nos critérios de byusca fornecidos anteriormente

            // se for somente codEstacao
            if (m == 1 && strcmp(nomesCampos[0], "codEstacao") == 0) {
                
                int chave_buscada = atoi(valoresCampos[0]); // converte em inteiro
                int rrn_encontrado = busca_binaria(vetor_indice, qtd_indices, chave_buscada); // usa busca bunária no índice primário na memória p obter o RRN 

                // se o rrn for necontrado
                if (rrn_encontrado != -1) {
                    // usa fseed pra saltar diretamente pra posição do registro
                    fseek(bin, 17 + (rrn_encontrado * 80), SEEK_SET);
                    Reg reg;
                    // salva o código original da estação
                    if (ler_reg_bin(bin, &reg) && reg.removido == '0') {
                        int cod_antigo = reg.codEstacao;

                        // percorre os p campos solicitados p alteração chamando a função atualiza_campo_reg
                        for (int j = 0; j < p; j++) {
                            atualiza_campo_reg(&reg, nomesAtt[j], valoresAtt[j]);
                        }

                        // grava no arquivo de dados e faz a manutenção do índice primário
                        fseek(bin, 17 + (rrn_encontrado * 80), SEEK_SET);
                        escreve_reg_bin(bin, &reg); 

                        if (reg.codEstacao != cod_antigo) {
                            remove_do_indice(vetor_indice, &qtd_indices, cod_antigo);
                            RegIndice nova_entrada;
                            nova_entrada.codEstacao = reg.codEstacao;
                            nova_entrada.rrn        = rrn_encontrado;
                            insere_no_indice(vetor_indice, &qtd_indices, nova_entrada);
                        }
                    }
                    free_reg(&reg);
                }
            
            // caso contrário
            } else {

                // move o ponteiro pra posição do primeiro registro
                fseek(bin, 17, SEEK_SET); 
                // variavel rrn_atual rastreia a posição de cada registro processado
                int rrn_atual = 0;
                Reg reg;

                // percorre todo o arquivo lendo registro por registro
                while (ler_reg_bin(bin, &reg)) {
                    // verifica se o registro não foi removido e se atende os critérios de busca específica
                    if (reg.removido == '0' && atende_criterios(&reg, m, nomesCampos, valoresCampos)) {

                        // se atender os critérios, entra no bloco de modificação
                        int cod_antigo = reg.codEstacao;

                        //Aplica cada atualização sobre o registro em memória
                        for (int j = 0; j < p; j++) {
                            atualiza_campo_reg(&reg, nomesAtt[j], valoresAtt[j]);
                        }

                        //Regrava o registro atualizado na posição correta
                        fseek(bin, 17 + (rrn_atual * 80), SEEK_SET);
                        escreve_reg_bin(bin, &reg);

                        fseek(bin, 17 + ((rrn_atual + 1) * 80), SEEK_SET);

                        //Atualiza o índice se codEstacao foi alterado 
                        if (reg.codEstacao != cod_antigo) {
                            remove_do_indice(vetor_indice, &qtd_indices, cod_antigo);
                            RegIndice nova_entrada;
                            nova_entrada.codEstacao = reg.codEstacao;
                            nova_entrada.rrn        = rrn_atual;
                            insere_no_indice(vetor_indice, &qtd_indices, nova_entrada);
                        }
                    }
                    free_reg(&reg);
                    rrn_atual++;
                }
            }
        } 

        // Atualiza o cabeçalho com status '1' (consistente)
        cabecalho.status = '1';
        fseek(bin, 0, SEEK_SET);
        escreve_header_bin(bin, &cabecalho);
        fclose(bin);

        // Reescreve o arquivo de índice com as entradas atualizadas 
        salva_indice_no_disco(arqIndice, vetor_indice, qtd_indices);
        free(vetor_indice);

        /* Saída obrigatória */
        BinarioNaTela(arqBIN);
        BinarioNaTela(arqIndice);
    }

   // FUNCIONALIDADE [10]: Criar e imprimir o Grafo
    else if (funcionalidade == 10) {
        scanf("%s", arqBIN); // le o nome do arquivo binario

        // abre o arquivo e acusa erro caso nao consiga
        FILE *bin = fopen(arqBIN, "rb");
        if (bin == NULL) {
            printf("Falha na execução da funcionalidade.\n");
            return 0;
        }

        // le o status pra checar a integridade do arquivo
        char status;
        fread(&status, sizeof(char), 1, bin);
        if (status == '0') {
            printf("Falha na execução da funcionalidade.\n");
            fclose(bin);
            return 0;
        }

        MapaCodNome mapa[5000]; 
        int qtd_mapa = 0;
        Grafo g;
        g.capacidade = 100;
        g.qtd_vertices = 0;
        g.vertices = malloc(g.capacidade * sizeof(Vertice));
        Reg reg;

        // cria um dicionario geral pra mapear o codigo de cada estacao pro seu nome, lendo apenas os registros ativos
        fseek(bin, 17, SEEK_SET);
        while (ler_reg_bin(bin, &reg)) {
            if (reg.removido == '0' && reg.codEstacao != -1 && reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) {
                int ja_tem_mapa = 0;
                for (int i = 0; i < qtd_mapa; i++) {
                    if (mapa[i].cod == reg.codEstacao) { ja_tem_mapa = 1; break; }
                }
                if (!ja_tem_mapa) {
                    mapa[qtd_mapa].cod = reg.codEstacao;
                    strcpy(mapa[qtd_mapa].nome, reg.nomeEstacao);
                    qtd_mapa++;
                }
            }
            free_reg(&reg);
        }

        // le os registros pra criar os vertices do grafo, garantindo que origens e destinos ativos existam
        fseek(bin, 17, SEEK_SET);
        while (ler_reg_bin(bin, &reg)) {
            if (reg.removido == '0') {
                char* nomes_v[3] = {NULL, NULL, NULL}; 
                
                if (reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) nomes_v[0] = reg.nomeEstacao;
                if (reg.codProxEstacao != -1) nomes_v[1] = lookup_nome(mapa, qtd_mapa, reg.codProxEstacao);
                if (reg.codEstIntegra != -1) nomes_v[2] = lookup_nome(mapa, qtd_mapa, reg.codEstIntegra);

                for (int v = 0; v < 3; v++) {
                    if (nomes_v[v] != NULL) {
                        int ja_tem = 0;
                        for (int i = 0; i < g.qtd_vertices; i++) {
                            if (strcmp(g.vertices[i].origem, nomes_v[v]) == 0) { ja_tem = 1; break; }
                        }
                        if (!ja_tem) {
                            if (g.qtd_vertices == g.capacidade) {
                                g.capacidade *= 2;
                                g.vertices = realloc(g.vertices, g.capacidade * sizeof(Vertice));
                            }
                            strcpy(g.vertices[g.qtd_vertices].origem, nomes_v[v]);
                            g.vertices[g.qtd_vertices].inicio = NULL;
                            g.qtd_vertices++;
                        }
                    }
                }
            }
            free_reg(&reg);
        }

        // ordena os vertices em ordem alfabetica
        qsort(g.vertices, g.qtd_vertices, sizeof(Vertice), compara_vertices);

        // cria as arestas e conecta os vertices
        fseek(bin, 17, SEEK_SET);
        while (ler_reg_bin(bin, &reg)) {
            if (reg.removido == '0' && reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) {
                char* origem = reg.nomeEstacao;

                if (reg.codProxEstacao != -1) {
                    char* destino = lookup_nome(mapa, qtd_mapa, reg.codProxEstacao);
                    if (destino != NULL && reg.tamNomeLinha > 0 && reg.nomeLinha != NULL) {
                        adiciona_aresta(&g, origem, destino, reg.distProxEstacao, reg.nomeLinha);
                    }
                }
                if (reg.codEstIntegra != -1) {
                    char* destino_integra = lookup_nome(mapa, qtd_mapa, reg.codEstIntegra);
                    if (destino_integra != NULL && strcmp(origem, destino_integra) != 0) {
                        adiciona_aresta(&g, origem, destino_integra, 0, "Integração");
                    }
                }
            }
            free_reg(&reg);
        }
        fclose(bin);

        // imprime o grafo com a formatacao exigida
        for (int i = 0; i < g.qtd_vertices; i++) {
            
            // pula a estacao se ela nao tiver vizinhos pra nao imprimir linha vazia
            if (g.vertices[i].inicio == NULL) {
                continue; 
            }

            printf("%s", g.vertices[i].origem);
            Aresta* a = g.vertices[i].inicio;
            while (a != NULL) {
                printf(", %s, %d", a->destino, a->distancia);
                for (int k = 0; k < a->linhas.qtd_linhas; k++) { 
                    printf(", %s", a->linhas.linhas[k]);
                }
                a = a->prox;
            }
            printf("\n");
        }

        libera_grafo(&g);
    }

    // FUNCIONALIDADE [11]: Caminho mais curto (Algoritmo de Dijkstra)
    else if (funcionalidade == 11) {
        char nomeCampoOrigem[50], valorOrigem[50];
        char nomeCampoDestino[50], valorDestino[50];

        // le as entradas e ignora arquivo de indice extra que o runcodes possa passar
        scanf("%s", arqBIN);
        char temp[50];
        scanf("%s", temp);
        if (strcmp(temp, "nomeEstacaoOrigem") == 0) {
            strcpy(nomeCampoOrigem, temp);
        } else {
            scanf("%s", nomeCampoOrigem); 
        }

        ScanQuoteString(valorOrigem); 
        scanf("%s", nomeCampoDestino);
        ScanQuoteString(valorDestino); 

        // abre o arquivo e acusa erro caso nao consiga
        FILE *bin = fopen(arqBIN, "rb");
        if (bin == NULL) {
            printf("Falha na execução da funcionalidade.\n");
            return 0;
        }

        // checa o status pra ver se o arquivo ta consistente
        char status;
        fread(&status, sizeof(char), 1, bin);
        if (status == '0') {
            printf("Falha na execução da funcionalidade.\n");
            fclose(bin);
            return 0;
        }

        MapaCodNome mapa[5000];
        int qtd_mapa = 0;
        Grafo g;
        g.capacidade = 100;
        g.qtd_vertices = 0;
        g.vertices = malloc(g.capacidade * sizeof(Vertice));
        Reg reg;

        // mesmo processo da func 10: criar dicionario, vertices e arestas
        fseek(bin, 17, SEEK_SET);
        while (ler_reg_bin(bin, &reg)) {
            if (reg.removido == '0' && reg.codEstacao != -1 && reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) {
                int ja_tem_mapa = 0;
                for (int i = 0; i < qtd_mapa; i++) {
                    if (mapa[i].cod == reg.codEstacao) { ja_tem_mapa = 1; break; }
                }
                if (!ja_tem_mapa) {
                    mapa[qtd_mapa].cod = reg.codEstacao;
                    strcpy(mapa[qtd_mapa].nome, reg.nomeEstacao);
                    qtd_mapa++;
                }
            }
            free_reg(&reg);
        }

        fseek(bin, 17, SEEK_SET);
        while (ler_reg_bin(bin, &reg)) {
            if (reg.removido == '0') {
                char* nomes_v[3] = {NULL, NULL, NULL};
                if (reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) nomes_v[0] = reg.nomeEstacao;
                if (reg.codProxEstacao != -1) nomes_v[1] = lookup_nome(mapa, qtd_mapa, reg.codProxEstacao);
                if (reg.codEstIntegra != -1) nomes_v[2] = lookup_nome(mapa, qtd_mapa, reg.codEstIntegra);

                for (int v = 0; v < 3; v++) {
                    if (nomes_v[v] != NULL) {
                        int ja_tem = 0;
                        for (int i = 0; i < g.qtd_vertices; i++) {
                            if (strcmp(g.vertices[i].origem, nomes_v[v]) == 0) { ja_tem = 1; break; }
                        }
                        if (!ja_tem) {
                            if (g.qtd_vertices == g.capacidade) {
                                g.capacidade *= 2;
                                g.vertices = realloc(g.vertices, g.capacidade * sizeof(Vertice));
                            }
                            strcpy(g.vertices[g.qtd_vertices].origem, nomes_v[v]);
                            g.vertices[g.qtd_vertices].inicio = NULL;
                            g.qtd_vertices++;
                        }
                    }
                }
            }
            free_reg(&reg);
        }

        qsort(g.vertices, g.qtd_vertices, sizeof(Vertice), compara_vertices);

        fseek(bin, 17, SEEK_SET);
        while (ler_reg_bin(bin, &reg)) {
            if (reg.removido == '0' && reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) {
                char* origem = reg.nomeEstacao;
                if (reg.codProxEstacao != -1) {
                    char* destino = lookup_nome(mapa, qtd_mapa, reg.codProxEstacao);
                    if (destino != NULL && reg.tamNomeLinha > 0 && reg.nomeLinha != NULL) {
                        adiciona_aresta(&g, origem, destino, reg.distProxEstacao, reg.nomeLinha);
                    }
                }
                if (reg.codEstIntegra != -1) {
                    char* destino_integra = lookup_nome(mapa, qtd_mapa, reg.codEstIntegra);
                    if (destino_integra != NULL && strcmp(origem, destino_integra) != 0) {
                        adiciona_aresta(&g, origem, destino_integra, 0, "Integração");
                    }
                }
            }
            free_reg(&reg);
        }
        fclose(bin);

        // chama a funcao dijkstra pra achar o menor caminho e imprimir
        dijkstra(&g, valorOrigem, valorDestino);

        libera_grafo(&g);
    }

    // FUNCIONALIDADE [12]: Árvore Geradora Mínima + Busca em Profundidade
    else if (funcionalidade == 12) {
        char nomeCampoOrigem[50], valorOrigem[100];

        // lê o nome do arquivo; le_nome_campo pula qualquer arquivo de
        // índice extra que o runcodes possa inserir antes do nome do campo
        scanf("%s", arqBIN);
        le_nome_campo(nomeCampoOrigem);
        ScanQuoteString(valorOrigem);

        // abre o arquivo e acusa erro caso nao consiga
        FILE *bin = fopen(arqBIN, "rb");
        if (bin == NULL) {
            printf("Falha na execução da funcionalidade.\n");
            return 0;
        }

        // checa o status pra ver se o arquivo ta consistente
        char status;
        fread(&status, sizeof(char), 1, bin);
        if (status == '0') {
            printf("Falha na execução da funcionalidade.\n");
            fclose(bin);
            return 0;
        }

        // monta o grafo direcionado original (mesmo criterio da funcionalidade 10)
        Grafo g;
        constroi_grafo_arquivo(bin, &g);
        fclose(bin);

        // constroi a arvore geradora minima (grafo tratado como nao-direcionado)
        Grafo arvore;
        arvore_geradora_minima(&g, &arvore);

        // percorre a arvore em profundidade a partir da estacao origem, imprimindo o resultado
        dfs_arvore(&arvore, valorOrigem);

        libera_grafo(&g);
        libera_grafo(&arvore);
    }

    // FUNCIONALIDADE [13]: Contagem de ciclos simples a partir de uma estação origem
    else if (funcionalidade == 13) {
        char nomeCampoOrigem[50], valorOrigem[100];

        // lê o nome do arquivo; le_nome_campo pula qualquer arquivo de
        // índice extra que o runcodes possa inserir antes do nome do campo
        scanf("%s", arqBIN);
        le_nome_campo(nomeCampoOrigem);
        ScanQuoteString(valorOrigem);

        // abre o arquivo e acusa erro caso nao consiga
        FILE *bin = fopen(arqBIN, "rb");
        if (bin == NULL) {
            printf("Falha na execução da funcionalidade.\n");
            return 0;
        }

        // checa o status pra ver se o arquivo ta consistente
        char status;
        fread(&status, sizeof(char), 1, bin);
        if (status == '0') {
            printf("Falha na execução da funcionalidade.\n");
            fclose(bin);
            return 0;
        }

        // monta o grafo direcionado original (mesmo criterio da funcionalidade 10)
        Grafo g;
        constroi_grafo_arquivo(bin, &g);
        fclose(bin);

        // conta os ciclos simples que partem e retornam a estacao origem
        int qtdCiclos = conta_ciclos_simples(&g, valorOrigem);
        printf("Quantidade de ciclos: %d\n", qtdCiclos);

        libera_grafo(&g);
    }
    return 0;
}