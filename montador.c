#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // Para uint32_t, uint8_t

// --- Estrutura para armazenar R�tulos ---
#define MAX_ROTULOS 100 // N�mero m�ximo de r�tulos que o montador pode lidar
typedef struct {
    char nome[50];    // Nome do r�tulo
    int endereco;     // Endere�o (em bytes) do r�tulo
} Rotulo;

Rotulo rotulos[MAX_ROTULOS];
int contador_rotulos = 0;

// Fun��o para adicionar um r�tulo � tabela
void adicionar_rotulo(const char *nome_rotulo, int endereco_rotulo) {
    if (contador_rotulos < MAX_ROTULOS) {
        strcpy(rotulos[contador_rotulos].nome, nome_rotulo);
        rotulos[contador_rotulos].endereco = endereco_rotulo;
        contador_rotulos++;
    } else {
        fprintf(stderr, "Erro: Limite de r�tulos excedido. Aumente MAX_ROTULOS.\n");
    }
}

// Fun��o para buscar o endere�o de um r�tulo
int buscar_endereco_rotulo(const char *nome_rotulo) {
    for (int i = 0; i < contador_rotulos; i++) {
        if (strcmp(rotulos[i].nome, nome_rotulo) == 0) {
            return rotulos[i].endereco;
        }
    }
    return -1; // R�tulo n�o encontrado
}

// --- Fun��es Auxiliares ---
// Converte o nome de um registrador (ABI ou xN) para seu n�mero (0-31)
int obter_numero_registrador(const char *nome_reg) {
    if (strcmp(nome_reg, "zero") == 0) return 0;
    if (strcmp(nome_reg, "ra") == 0) return 1;
    if (strcmp(nome_reg, "sp") == 0) return 2;
    if (strcmp(nome_reg, "gp") == 0) return 3;
    if (strcmp(nome_reg, "tp") == 0) return 4;
    if (strcmp(nome_reg, "t0") == 0) return 5;
    if (strcmp(nome_reg, "t1") == 0) return 6;
    if (strcmp(nome_reg, "t2") == 0) return 7;
    if (strcmp(nome_reg, "s0") == 0 || strcmp(nome_reg, "fp") == 0) return 8; // fp � alias para s0
    if (strcmp(nome_reg, "s1") == 0) return 9;
    if (strcmp(nome_reg, "a0") == 0) return 10;
    if (strcmp(nome_reg, "a1") == 0) return 11;
    if (strcmp(nome_reg, "a2") == 0) return 12;
    if (strcmp(nome_reg, "a3") == 0) return 13;
    if (strcmp(nome_reg, "a4") == 0) return 14;
    if (strcmp(nome_reg, "a5") == 0) return 15;
    if (strcmp(nome_reg, "a6") == 0) return 16;
    if (strcmp(nome_reg, "a7") == 0) return 17;
    if (strcmp(nome_reg, "s2") == 0) return 18;
    if (strcmp(nome_reg, "s3") == 0) return 19;
    if (strcmp(nome_reg, "s4") == 0) return 20;
    if (strcmp(nome_reg, "s5") == 0) return 21;
    if (strcmp(nome_reg, "s6") == 0) return 22;
    if (strcmp(nome_reg, "s7") == 0) return 23;
    if (strcmp(nome_reg, "s8") == 0) return 24;
    if (strcmp(nome_reg, "s9") == 0) return 25;
    if (strcmp(nome_reg, "s10") == 0) return 26;
    if (strcmp(nome_reg, "s11") == 0) return 27;
    if (strcmp(nome_reg, "t3") == 0) return 28;
    if (strcmp(nome_reg, "t4") == 0) return 29;
    if (strcmp(nome_reg, "t5") == 0) return 30;
    if (strcmp(nome_reg, "t6") == 0) return 31;
    
    // Tenta converter de "xN" para N
    if (nome_reg[0] == 'x') {
        int num_reg = atoi(&nome_reg[1]);
        if (num_reg >= 0 && num_reg <= 31) {
            return num_reg;
        }
    }
    return -1; // Nome de registrador inv�lido
}

// Converte um n�mero decimal para uma string bin�ria com um n�mero espec�fico de bits
void dec_para_bin_n_bits(int num_bits, int decimal, char *string_binaria) {
    for (int i = num_bits - 1; i >= 0; i--) {
        string_binaria[num_bits - 1 - i] = ((decimal >> i) & 1) ? '1' : '0';
    }
    string_binaria[num_bits] = '\0'; // Terminador nulo
}

// --- Fun��o da Primeira Passagem (Coleta de R�tulos) ---
// L� o arquivo de entrada, identifica todos os r�tulos e armazena seus nomes e endere�os.
// Endere�os s�o contados em bytes, assumindo 4 bytes por instru��o.
void primeira_passagem(const char *nome_arquivo_entrada) {
    FILE *arquivo_entrada = fopen(nome_arquivo_entrada, "r");
    if (arquivo_entrada == NULL) {
        perror("Erro ao abrir o arquivo de entrada na primeira passagem");
        exit(1);
    }

    char linha[256];
    int endereco_atual = 0; // Endere�o da instru��o atual em bytes

    while (fgets(linha, sizeof(linha), arquivo_entrada) != NULL) {
        // Remove nova linha e espa�os em branco no final
        linha[strcspn(linha, "\n\r")] = 0; 
        char *inicio_linha = linha;
        while (*inicio_linha == ' ' || *inicio_linha == '\t') inicio_linha++; // Pula espa�os no in�cio

        // Ignora linhas vazias ou coment�rios
        if (strlen(inicio_linha) == 0 || inicio_linha[0] == '#' || inicio_linha[0] == ';') {
            continue;
        }
        
        // Cria uma c�pia da linha para tokeniza��o, pois strtok modifica a string
        char linha_temporaria[256];
        strcpy(linha_temporaria, inicio_linha);

        char *token = strtok(linha_temporaria, " ,\t"); // Delimitadores: espa�o, v�rgula, tabula��o
        if (token == NULL) continue;

        // Verifica se o primeiro token � um r�tulo (termina com ':')
        char *ponteiro_dois_pontos = strchr(token, ':');
        if (ponteiro_dois_pontos != NULL) {
            *ponteiro_dois_pontos = '\0'; // Remove o ':' para obter o nome do r�tulo
            adicionar_rotulo(token, endereco_atual);
            
            // Avan�a para o pr�ximo token, que seria a instru��o (se houver na mesma linha)
            token = strtok(NULL, " ,\t"); 
            if (token == NULL) { // R�tulo em uma linha pr�pria
                continue; 
            }
        }
        // Se n�o era um r�tulo ou se havia uma instru��o ap�s o r�tulo,
        // esta linha cont�m uma instru��o que ocupar� 4 bytes.
        endereco_atual += 4;
    }
    fclose(arquivo_entrada);
}

// --- Fun��o da Segunda Passagem (Gera��o do C�digo Bin�rio) ---
// L� o arquivo de entrada novamente, traduz cada instru��o para seu formato bin�rio
// e escreve o resultado no arquivo de sa�da.
void segunda_passagem(const char *nome_arquivo_entrada, const char *nome_arquivo_saida) {
    FILE *arquivo_entrada = NULL;
    FILE *arquivo_saida = NULL;
    char linha[256];
    int endereco_atual = 0; // Endere�o da instru��o atual em bytes

    arquivo_entrada = fopen(nome_arquivo_entrada, "r");
    if (arquivo_entrada == NULL) {
        perror("Erro ao abrir o arquivo de entrada na segunda passagem");
        return;
    }

    arquivo_saida = fopen(nome_arquivo_saida, "w");
    if (arquivo_saida == NULL) {
        perror("Erro ao abrir o arquivo de sa�da");
        fclose(arquivo_entrada);
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo_entrada) != NULL) {
        // Remove nova linha e espa�os em branco no final
        linha[strcspn(linha, "\n\r")] = 0;
        char *inicio_linha = linha;
        while (*inicio_linha == ' ' || *inicio_linha == '\t') inicio_linha++;

        // Ignora linhas vazias ou coment�rios
        if (strlen(inicio_linha) == 0 || inicio_linha[0] == '#' || inicio_linha[0] == ';') {
            continue;
        }
        
        char linha_temporaria[256];
        strcpy(linha_temporaria, inicio_linha);

        char *token = strtok(linha_temporaria, " ,\t");
        if (token == NULL) continue;

        char *instrucao_mnemonica = token;
        // Se o primeiro token for um r�tulo, pegue o pr�ximo token como a instru��o
        if (strchr(instrucao_mnemonica, ':') != NULL) {
            instrucao_mnemonica = strtok(NULL, " ,\t");
            if (instrucao_mnemonica == NULL) { // Linha continha apenas um r�tulo
                continue;
            }
        }
        
        uint32_t binario_instrucao = 0; // Valor bin�rio da instru��o
        int instrucao_valida = 1;       // Flag para verificar se a instru��o foi processada corretamente

        // --- Instru��es tipo R ---
        // Formato: funct7 | rs2 | rs1 | funct3 | rd | opcode
        if (strcmp(instrucao_mnemonica, "add") == 0) {
            char *rd_txt = strtok(NULL, " ,\t"); char *rs1_txt = strtok(NULL, " ,\t"); char *rs2_txt = strtok(NULL, " ,\t");
            int rd = obter_numero_registrador(rd_txt); int rs1 = obter_numero_registrador(rs1_txt); int rs2 = obter_numero_registrador(rs2_txt);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inv�lido para 'add'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else binario_instrucao = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instrucao_mnemonica, "sub") == 0) {
            char *rd_txt = strtok(NULL, " ,\t"); char *rs1_txt = strtok(NULL, " ,\t"); char *rs2_txt = strtok(NULL, " ,\t");
            int rd = obter_numero_registrador(rd_txt); int rs1 = obter_numero_registrador(rs1_txt); int rs2 = obter_numero_registrador(rs2_txt);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inv�lido para 'sub'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else binario_instrucao = (0b0100000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instrucao_mnemonica, "xor") == 0) {
            char *rd_txt = strtok(NULL, " ,\t"); char *rs1_txt = strtok(NULL, " ,\t"); char *rs2_txt = strtok(NULL, " ,\t");
            int rd = obter_numero_registrador(rd_txt); int rs1 = obter_numero_registrador(rs1_txt); int rs2 = obter_numero_registrador(rs2_txt);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inv�lido para 'xor'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else binario_instrucao = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b100 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instrucao_mnemonica, "or") == 0) {
            char *rd_txt = strtok(NULL, " ,\t"); char *rs1_txt = strtok(NULL, " ,\t"); char *rs2_txt = strtok(NULL, " ,\t");
            int rd = obter_numero_registrador(rd_txt); int rs1 = obter_numero_registrador(rs1_txt); int rs2 = obter_numero_registrador(rs2_txt);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inv�lido para 'or'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else binario_instrucao = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b110 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instrucao_mnemonica, "and") == 0) {
            char *rd_txt = strtok(NULL, " ,\t"); char *rs1_txt = strtok(NULL, " ,\t"); char *rs2_txt = strtok(NULL, " ,\t");
            int rd = obter_numero_registrador(rd_txt); int rs1 = obter_numero_registrador(rs1_txt); int rs2 = obter_numero_registrador(rs2_txt);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inv�lido para 'and'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else binario_instrucao = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b111 << 12) | (rd << 7) | 0b0110011;
        // Instru��es de shift (tipo R, mas com shamt no lugar de rs2 para o opcode espec�fico)
        // Formato SLLI/SRLI: 0000000 | shamt | rs1 | funct3 | rd | OPCODE (OP-IMM-SHIFT)
        } else if (strcmp(instrucao_mnemonica, "slli") == 0) { // RV32I shamt � de 5 bits [24:20]
            char *rd_txt = strtok(NULL, " ,\t"); char *rs1_txt = strtok(NULL, " ,\t"); char *shamt_txt = strtok(NULL, " ,\t");
            int rd = obter_numero_registrador(rd_txt); int rs1 = obter_numero_registrador(rs1_txt); int shamt = atoi(shamt_txt);
            if (rd == -1 || rs1 == -1 || shamt < 0 || shamt > 31) { fprintf(stderr, "Erro em 0x%04X: Par�metro inv�lido para 'slli'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else binario_instrucao = (0b0000000 << 25) | (shamt << 20) | (rs1 << 15) | (0b001 << 12) | (rd << 7) | 0b0010011; // Opcode OP-IMM
        } else if (strcmp(instrucao_mnemonica, "srli") == 0) { // RV32I shamt � de 5 bits [24:20]
            char *rd_txt = strtok(NULL, " ,\t"); char *rs1_txt = strtok(NULL, " ,\t"); char *shamt_txt = strtok(NULL, " ,\t");
            int rd = obter_numero_registrador(rd_txt); int rs1 = obter_numero_registrador(rs1_txt); int shamt = atoi(shamt_txt);
            if (rd == -1 || rs1 == -1 || shamt < 0 || shamt > 31) { fprintf(stderr, "Erro em 0x%04X: Par�metro inv�lido para 'srli'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else binario_instrucao = (0b0000000 << 25) | (shamt << 20) | (rs1 << 15) | (0b101 << 12) | (rd << 7) | 0b0010011; // Opcode OP-IMM

        // --- Instru��es tipo I ---
        // Formato: immediate[11:0] | rs1 | funct3 | rd | opcode
        } else if (strcmp(instrucao_mnemonica, "addi") == 0) {
            char *rd_txt = strtok(NULL, " ,\t"); char *rs1_txt = strtok(NULL, " ,\t"); char *imm_txt = strtok(NULL, " ,\t");
            int rd = obter_numero_registrador(rd_txt); int rs1 = obter_numero_registrador(rs1_txt); int imm = atoi(imm_txt);
            if (rd == -1 || rs1 == -1 || imm < -2048 || imm > 2047) { fprintf(stderr, "Erro em 0x%04X: Par�metro inv�lido para 'addi'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else binario_instrucao = ((imm & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0010011;
        } else if (strcmp(instrucao_mnemonica, "lw") == 0) { // Formato: lw rd, offset(rs1)
            char *rd_txt = strtok(NULL, " ,\t"); char *offset_rs1_txt = strtok(NULL, " ,\t"); // Pega "offset(rs1)"
            char *offset_txt = strtok(offset_rs1_txt, "("); 
            char *rs1_txt = strtok(NULL, ")");
            if (rd_txt == NULL || offset_txt == NULL || rs1_txt == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inv�lido para 'lw'. Use 'lw rd, offset(rs1)'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else {
                int rd = obter_numero_registrador(rd_txt); int rs1 = obter_numero_registrador(rs1_txt); int offset = atoi(offset_txt);
                if (rd == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Par�metro inv�lido para 'lw'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
                else binario_instrucao = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b010 << 12) | (rd << 7) | 0b0000011; // Opcode LOAD
            }
        } else if (strcmp(instrucao_mnemonica, "jalr") == 0) { // Formato: jalr rd, rs1, offset  ou jalr rd, offset(rs1)
            char *rd_txt = strtok(NULL, " ,\t");
            char *arg2_txt = strtok(NULL, " ,\t"); // Pode ser rs1 ou offset(rs1)
            char *arg3_txt = strtok(NULL, " ,\t"); // Pode ser offset ou NULL
            
            int rd = -1, rs1 = -1, offset = 0;
            if (rd_txt) rd = obter_numero_registrador(rd_txt);

            if (arg3_txt != NULL) { // Formato jalr rd, rs1, offset
                if (arg2_txt) rs1 = obter_numero_registrador(arg2_txt);
                offset = atoi(arg3_txt);
            } else if (arg2_txt != NULL) { // Formato jalr rd, offset(rs1) ou jalr rd, rs1 (offset=0)
                char *temp_rs1_txt = strchr(arg2_txt, '(');
                if (temp_rs1_txt != NULL) { // Formato offset(rs1)
                    *temp_rs1_txt = '\0'; // Separa offset
                    offset = atoi(arg2_txt);
                    rs1 = obter_numero_registrador(temp_rs1_txt + 1);
                    // Remove ')' do final de rs1_txt se presente
                    char* fecha_parenteses = strchr(temp_rs1_txt + 1, ')');
                    if (fecha_parenteses) *fecha_parenteses = '\0';

                } else { // Formato jalr rd, rs1 (offset impl�cito 0)
                    rs1 = obter_numero_registrador(arg2_txt);
                    offset = 0;
                }
            }

            if (rd == -1 || rs1 == -1 || offset < -2048 || offset > 2047) {
                fprintf(stderr, "Erro em 0x%04X: Par�metro inv�lido para 'jalr'. Linha: %s\n", endereco_atual, inicio_linha);
                instrucao_valida = 0;
            } else {
                binario_instrucao = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b1100111; // Opcode JALR
            }
        
        // --- Instru��es tipo S ---
        // Formato: imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode
        } else if (strcmp(instrucao_mnemonica, "sw") == 0) { // Formato: sw rs2, offset(rs1)
            char *rs2_txt = strtok(NULL, " ,\t"); 
            char *offset_rs1_txt = strtok(NULL, " ,\t"); // Pega "offset(rs1)"
            char *offset_txt = strtok(offset_rs1_txt, "(");
            char *rs1_txt = strtok(NULL, ")");
            if (rs2_txt == NULL || offset_txt == NULL || rs1_txt == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inv�lido para 'sw'. Use 'sw rs2, offset(rs1)'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else {
                int rs2 = obter_numero_registrador(rs2_txt); int rs1 = obter_numero_registrador(rs1_txt); int offset = atoi(offset_txt);
                if (rs2 == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Par�metro inv�lido para 'sw'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
                else {
                    uint32_t imm_11_5 = (offset >> 5) & 0x7F; // bits 11 a 5 do imediato
                    uint32_t imm_4_0  = offset & 0x1F;        // bits 4 a 0 do imediato
                    binario_instrucao = (imm_11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b010 << 12) | (imm_4_0 << 7) | 0b0100011; // Opcode STORE
                }
            }

        // --- Instru��es tipo B ---
        // Formato: imm[12] | imm[10:5] | rs2 | rs1 | funct3 | imm[4:1] | imm[11] | opcode
        } else if (strcmp(instrucao_mnemonica, "beq") == 0 || strcmp(instrucao_mnemonica, "bne") == 0) {
            char *rs1_txt = strtok(NULL, " ,\t"); char *rs2_txt = strtok(NULL, " ,\t"); char *rotulo_destino_txt = strtok(NULL, " ,\t");
            if (rs1_txt == NULL || rs2_txt == NULL || rotulo_destino_txt == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inv�lido para '%s'. Linha: %s\n", endereco_atual, instrucao_mnemonica, inicio_linha); instrucao_valida = 0; }
            else {
                int rs1 = obter_numero_registrador(rs1_txt); int rs2 = obter_numero_registrador(rs2_txt);
                int endereco_destino = buscar_endereco_rotulo(rotulo_destino_txt);
                if (rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inv�lido para '%s'. Linha: %s\n", endereco_atual, instrucao_mnemonica, inicio_linha); instrucao_valida = 0; }
                else if (endereco_destino == -1) { fprintf(stderr, "Erro em 0x%04X: R�tulo '%s' n�o encontrado. Linha: %s\n", endereco_atual, rotulo_destino_txt, inicio_linha); instrucao_valida = 0; }
                else {
                    int deslocamento = endereco_destino - endereco_atual; 
                    // O deslocamento para branches � em m�ltiplos de 2 bytes (1 bit impl�cito 0 � direita)
                    // O campo imediato de 13 bits (incluindo o impl�cito) pode representar de -4096 a +4094 bytes.
                    if (deslocamento % 2 != 0) { fprintf(stderr, "Erro em 0x%04X: Deslocamento do branch '%s' n�o � m�ltiplo de 2. Linha: %s\n", endereco_atual, rotulo_destino_txt, inicio_linha); instrucao_valida = 0; }
                    else if (deslocamento < -4096 || deslocamento > 4094) { fprintf(stderr, "Erro em 0x%04X: Deslocamento do branch '%s' fora do alcance. Linha: %s\n", endereco_atual, rotulo_destino_txt, inicio_linha); instrucao_valida = 0;}
                    else {
                        // Extra��o dos bits do imediato conforme formato B
                        uint32_t imm_12   = (deslocamento >> 12) & 0x1;  // bit 12 do deslocamento
                        uint32_t imm_10_5 = (deslocamento >> 5)  & 0x3F; // bits 10 a 5
                        uint32_t imm_4_1  = (deslocamento >> 1)  & 0xF;  // bits 4 a 1
                        uint32_t imm_11   = (deslocamento >> 11) & 0x1;  // bit 11
                        
                        uint32_t funct3 = (strcmp(instrucao_mnemonica, "beq") == 0) ? 0b000 : 0b001; // beq ou bne

                        binario_instrucao = (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (imm_4_1 << 8) | (imm_11 << 7) | 0b1100011; // Opcode BRANCH
                    }
                }
            }
        
        // --- Instru��es tipo U ---
        // Formato: immediate[31:12] | rd | opcode
        } else if (strcmp(instrucao_mnemonica, "lui") == 0) {
            char *rd_txt = strtok(NULL, " ,\t"); char *imm_txt = strtok(NULL, " ,\t");
            if (rd_txt == NULL || imm_txt == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inv�lido para 'lui'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else {
                int rd = obter_numero_registrador(rd_txt); 
                long imm_val = strtol(imm_txt, NULL, 0); // Permite hex (0x), octal (0), decimal
                
                // LUI carrega um imediato de 20 bits nos bits superiores (31 a 12) de rd.
                // O valor fornecido (imm_val) � o que vai para esses bits.
                if (rd == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inv�lido para 'lui'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
                // Verifica se o imediato cabe em 20 bits (signed or unsigned interpretation of immediate can vary by assembler convention, typically it's the direct 20-bit value)
                // Aqui, vamos assumir que o valor fornecido � o valor exato dos 20 bits superiores.
                else binario_instrucao = ((imm_val & 0xFFFFF) << 12) | (rd << 7) | 0b0110111; // Opcode LUI
            }

        // --- Instru��es tipo J (jal) ---
        // Formato: imm[20] | imm[10:1] | imm[11] | imm[19:12] | rd | opcode
        } else if (strcmp(instrucao_mnemonica, "jal") == 0) {
            char *rd_txt = strtok(NULL, " ,\t"); char *rotulo_destino_txt = strtok(NULL, " ,\t");
            if (rd_txt == NULL || rotulo_destino_txt == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inv�lido para 'jal'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
            else {
                int rd = obter_numero_registrador(rd_txt);
                int endereco_destino = buscar_endereco_rotulo(rotulo_destino_txt);
                if (rd == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inv�lido para 'jal'. Linha: %s\n", endereco_atual, inicio_linha); instrucao_valida = 0; }
                else if (endereco_destino == -1) { fprintf(stderr, "Erro em 0x%04X: R�tulo '%s' n�o encontrado. Linha: %s\n", endereco_atual, rotulo_destino_txt, inicio_linha); instrucao_valida = 0; }
                else {
                    int deslocamento = endereco_destino - endereco_atual;
                    // O deslocamento para JAL � em m�ltiplos de 2 bytes.
                    // O campo imediato de 21 bits (incluindo o impl�cito 0 � direita) pode representar +/- 1MB.
                    if (deslocamento % 2 != 0) { fprintf(stderr, "Erro em 0x%04X: Deslocamento do JAL '%s' n�o � m�ltiplo de 2. Linha: %s\n", endereco_atual, rotulo_destino_txt, inicio_linha); instrucao_valida = 0; }
                    // 20 bits para o imediato, deslocamento pode ser at� 2^20 * 2 bytes = +/-1MB
                    else if (deslocamento < -(1 << 20) || deslocamento >= (1 << 20) ) { fprintf(stderr, "Erro em 0x%04X: Deslocamento JAL '%s' fora do alcance. Linha: %s\n", endereco_atual, rotulo_destino_txt, inicio_linha); instrucao_valida = 0; }
                    else {
                        // Extra��o dos bits do imediato conforme formato J
                        // deslocamento j� � o valor em bytes.
                        uint32_t imm20    = (deslocamento >> 20) & 0x1;    // bit 20 do deslocamento
                        uint32_t imm10_1  = (deslocamento >> 1)  & 0x3FF;  // bits 10 a 1
                        uint32_t imm11    = (deslocamento >> 11) & 0x1;    // bit 11
                        uint32_t imm19_12 = (deslocamento >> 12) & 0xFF;   // bits 19 a 12
                        
                        binario_instrucao = (imm20 << 31) | (imm10_1 << 21) | (imm11 << 20) | (imm19_12 << 12) | (rd << 7) | 0b1101111; // Opcode JAL
                    }
                }
            }

        } else {
            fprintf(stderr, "Instru��o desconhecida em 0x%04X: '%s'. Linha: %s\n", endereco_atual, instrucao_mnemonica, inicio_linha);
            instrucao_valida = 0;
        }

        if (instrucao_valida) {
            char byte_binario_txt[9]; // 8 bits + terminador nulo

            // Ordem dos bytes: Little-endian (byte menos significativo primeiro)
            // Byte 0: bits 7-0 da instru��o
            // Byte 1: bits 15-8 da instru��o
            // Byte 2: bits 23-16 da instru��o
            // Byte 3: bits 31-24 da instru��o

            uint8_t byte0 = (binario_instrucao >> 0)  & 0xFF;
            uint8_t byte1 = (binario_instrucao >> 8)  & 0xFF;
            uint8_t byte2 = (binario_instrucao >> 16) & 0xFF;
            uint8_t byte3 = (binario_instrucao >> 24) & 0xFF;

            dec_para_bin_n_bits(8, byte0, byte_binario_txt);
            fprintf(arquivo_saida, "%s\n", byte_binario_txt);

            dec_para_bin_n_bits(8, byte1, byte_binario_txt);
            fprintf(arquivo_saida, "%s\n", byte_binario_txt);

            dec_para_bin_n_bits(8, byte2, byte_binario_txt);
            fprintf(arquivo_saida, "%s\n", byte_binario_txt);

            dec_para_bin_n_bits(8, byte3, byte_binario_txt);
            fprintf(arquivo_saida, "%s\n", byte_binario_txt);
            
            endereco_atual += 4; // Avan�a para o pr�ximo endere�o de instru��o
        } else {
            // Se a instru��o for inv�lida, n�o incremente o endere�o_atual aqui
            // pois a primeira passagem j� contou todas as instru��es v�lidas (ou linhas que pareciam instru��es).
            // Se uma instru��o for inv�lida na segunda passagem, algo deu errado ou o erro � na instru��o em si.
            // A contagem de endere�o_atual deve ser consistente com a primeira passagem.
            // Para simplificar, vamos avan�ar o endere�o_atual de qualquer forma para manter a sincronia,
            // mas idealmente, um erro aqui deveria parar a montagem ou ser tratado de forma mais robusta.
            // Para este exemplo, se uma instru��o for inv�lida, ela n�o ser� escrita no arquivo de sa�da
            // e o endere�o_atual continua sendo incrementado para a pr�xima linha do arquivo de entrada.
             endereco_atual += 4; // Mesmo que inv�lida, contamos para manter o endere�o_atual sincronizado com a primeira passagem
        }
    }

    fclose(arquivo_entrada);
    fclose(arquivo_saida);
    printf("Montagem conclu�da! Arquivo '%s' gerado.\n", nome_arquivo_saida);
}

int main(int argc, char *argv[]) {
    const char *nome_arquivo_entrada = NULL;
    const char *nome_arquivo_saida = NULL;

    if (argc == 2) { // Apenas o arquivo de entrada foi fornecido
        nome_arquivo_entrada = argv[1];
        nome_arquivo_saida = "resposta.txt"; // Nome padr�o para o arquivo de sa�da
        printf("INFO: Nome do arquivo de sa�da n�o fornecido. Usando '%s' como padr�o.\n\n", nome_arquivo_saida);
    } else if (argc == 3) { // Ambos os arquivos foram fornecidos
        nome_arquivo_entrada = argv[1];
        nome_arquivo_saida = argv[2];
    } else { // N�mero incorreto de argumentos
        fprintf(stderr, "Uso: %s <arquivo_entrada.asm> [nome_arquivo_saida.txt]\n", argv[0]);
        fprintf(stderr, "Se [nome_arquivo_saida.txt] n�o for especificado, ser� usado 'resposta.txt' por padr�o.\n");
        return 1;
    }

    // (Restante da fun��o main continua aqui...)
    // Imprime o nome do arquivo de entrada e seu conte�do
    printf("%s:\n", nome_arquivo_entrada);
    // ... (c�digo para imprimir arquivo de entrada) ...

    // Primeira Passagem: Coleta r�tulos
    primeira_passagem(nome_arquivo_entrada);

    // Segunda Passagem: Monta o c�digo e resolve r�tulos
    segunda_passagem(nome_arquivo_entrada, nome_arquivo_saida);

    // Imprime o nome do arquivo de sa�da e seu conte�do
    printf("\n%s:\n", nome_arquivo_saida);
    // ... (c�digo para imprimir arquivo de sa�da) ...

    return 0;
}
