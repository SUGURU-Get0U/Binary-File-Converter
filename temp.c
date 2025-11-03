#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    CEREAIS = 1, VERDURAS, FRUTAS, GORDURAS, PESCADOS,
    CARNES, LACTEOS, BEBIDAS, OVOS, ACUCARES,
    MISCELANEAS, INDUSTRIALIZADOS, PREPARADOS, LEGUMINOSAS, SEMENTES
} Categoria;

typedef struct {
    int codigo;
    char nome[50];
    float calorias;
    float proteinas;
    float carboidratos;
    float gorduras;
    float fibras;
} Alimento;

void removerQuebraLinha(char *str) {
    size_t len = strlen(str);
    if (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) { // condicional: se final é quebra
        while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) { // loop: remove todos os finais \n ou \r
            str[len-1] = '\0';
            len--;
        }
    }
}

/* Substitui vírgula por ponto para conversão float */
void substituirVirgulaPorPonto(char *str) {
    for (int i = 0; str[i] != '\0'; i++) { // loop: percorre string
        if (str[i] == ',') { // condicional: se encontrar vírgula
            str[i] = '.';
        }
    }
}

/* Remove aspas duplas no início/fim da string */
void removerAspas(char *str) {
    size_t len = strlen(str);
    if (len == 0) return; // condicional: se string vazia retorna
    if (str[0] == '"') { // condicional: se primeiro caractere é "
        memmove(str, str+1, strlen(str)); // move para frente
    }
    len = strlen(str);
    if (len > 0 && str[len-1] == '"') { // condicional: se último é "
        str[len-1] = '\0';
    }
}

/* Processa porcentagem: remove '%' e converte para float */
float processarPorcentagem(char *str) {
    removerAspas(str); // chama função que remove aspas
    size_t len = strlen(str);
    if (len > 0 && str[len-1] == '%') { // condicional: se último caractere é %
        str[len-1] = '\0';
    }
    substituirVirgulaPorPonto(str); // converte vírgula->ponto
    return atof(str); // converte para float
}

/* Converte nome textual da categoria para enum (busca por palavras-chave) */
Categoria nomeParaCategoria(const char *nome) {
    char tmp[200];
    strncpy(tmp, nome, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1] = '\0';
    // condicional em sequência: verifica substrings representativas
    if (strstr(tmp, "Cereais")) return CEREAIS;
    if (strstr(tmp, "Verduras") || strstr(tmp, "hortali")) return VERDURAS;
    if (strstr(tmp, "Frutas")) return FRUTAS;
    if (strstr(tmp, "Gorduras") || strstr(tmp, "oleo")) return GORDURAS;
    if (strstr(tmp, "Pescados") || strstr(tmp, "frutos")) return PESCADOS;
    if (strstr(tmp, "Carnes")) return CARNES;
    if (strstr(tmp, "Leite") || strstr(tmp, "lacteo")) return LACTEOS;
    if (strstr(tmp, "Bebid")) return BEBIDAS;
    if (strstr(tmp, "Ovos")) return OVOS;
    if (strstr(tmp, "acucar") || strstr(tmp, "açuca")) return ACUCARES;
    if (strstr(tmp, "Miscel") || strstr(tmp, "Miscelane")) return MISCELANEAS;
    if (strstr(tmp, "industrial")) return INDUSTRIALIZADOS;
    if (strstr(tmp, "prepar")) return PREPARADOS;
    if (strstr(tmp, "Legumin")) return LEGUMINOSAS;
    if (strstr(tmp, "Noz") || strstr(tmp, "semente")) return SEMENTES;
    return CEREAIS; // padrão caso não reconhecido
}

/* Converte inteiro para Categoria (validação simples) */
Categoria intParaCategoria(int cat_num) {
    if (cat_num >= CEREAIS && cat_num <= SEMENTES) { // condicional: intervalo válido
        return (Categoria)cat_num;
    }
    return CEREAIS; // padrão
}

/* Retorna nome legível da categoria */
const char* obterNomeCategoria(Categoria cat) {
    static const char *nomes[] = {
        "", "Cereais e derivados", "Verduras e hortaliças", "Frutas e derivados",
        "Gorduras e óleos", "Pescados e frutos do mar", "Carnes e derivados",
        "Leite e derivados", "Bebidas", "Ovos e derivados", "Produtos açucarados",
        "Miscelâneas", "Industrializados", "Preparados", "Leguminosas", "Nozes e sementes"
    };
    if (cat >= CEREAIS && cat <= SEMENTES) return nomes[cat];
    return "Desconhecida";
}
int lerDadosCSVParaBinario(const char* arquivoCSV, const char* arquivoBinario) {
    FILE *arquivo_csv, *arquivo_binario;
    Alimento alimento;
    int contador = 0;
    char linha[256]; // Buffer para leitura de uma linha do arquivo CSV
    
    arquivo_csv = fopen(arquivoCSV, "r");
    if (arquivo_csv == NULL) {
        printf("Erro: Não foi possível abrir o arquivo %s\n", arquivoCSV);
        return -1;
    }
    
    arquivo_binario = fopen(arquivoBinario, "wb");
    if (arquivo_binario == NULL) {
        printf("Erro: Não foi possível criar o arquivo %s\n", arquivoBinario);
        fclose(arquivo_csv);
        return -1;
    }

    // Ignorar a primeira linha do CSV (cabeçalho)
    if (fgets(linha, sizeof(linha), arquivo_csv) == NULL) {
        printf("Erro: Arquivo CSV vazio ou mal formatado\n");
        fclose(arquivo_csv);
        fclose(arquivo_binario);
        return -1;
    }

    // Leitura das linhas do arquivo CSV
    while (fgets(linha, sizeof(linha), arquivo_csv) != NULL) {
        // Usar sscanf para extrair os campos separados por vírgula
        if (sscanf(linha, "%d,%49[^,],%f,%f,%f,%f,%f", 
                   &alimento.codigo, 
                   alimento.nome, 
                   &alimento.calorias, 
                   &alimento.proteinas, 
                   &alimento.carboidratos, 
                   &alimento.gorduras, 
                   &alimento.fibras) == 7) {
            
            // Escrever o registro no arquivo binário
            if (fwrite(&alimento, sizeof(Alimento), 1, arquivo_binario) != 1) {
                printf("Erro ao escrever dados no arquivo binário\n");
                break;
            }
            contador++;
        } else {
            printf("Erro ao processar a linha: %s", linha);
        }
    }
    
    fclose(arquivo_csv);
    fclose(arquivo_binario);
    
    return contador;
}

void verificarArquivoBinario(const char* arquivoBinario) {
    FILE *arquivo;
    Alimento alimento;
    int contador = 0;
    
    arquivo = fopen(arquivoBinario, "rb");
    if (arquivo == NULL) {
        printf("Erro: Não foi possível abrir o arquivo binário para verificação\n");
        return;
    }
    
    printf("\n=== Verificação do Arquivo Binário ===\n");
    printf("Código | Nome                | Calorias | Proteínas | Carboidratos | Gorduras | Fibras\n");
    printf("-------|---------------------|----------|-----------|--------------|----------|--------\n");
    
    while (fread(&alimento, sizeof(Alimento), 1, arquivo) == 1) {
        printf("%-6d | %-19s | %8.2f | %9.2f | %12.2f | %8.2f | %6.2f\n",
               alimento.codigo,
               alimento.nome,
               alimento.calorias,
               alimento.proteinas,
               alimento.carboidratos,
               alimento.gorduras,
               alimento.fibras);
        contador++;
    }
    
    printf("\nTotal de alimentos lidos: %d\n", contador);
    fclose(arquivo);
}

int main() {
    const char* arquivo_entrada = "Tabela de Alimento_RA2.CSV";  // Arquivo CSV de entrada
    const char* arquivo_saida = "dados.bin";       // Arquivo binário de saída
    
    printf("=== Programa de Conversão de Dados de Alimentos ===\n");
    printf("Convertendo dados de %s para %s...\n", arquivo_entrada, arquivo_saida);
    
    int resultado = lerDadosCSVParaBinario(arquivo_entrada, arquivo_saida);
    
    if (resultado > 0) {
        printf(" Conversão realizada com sucesso!\n");
        printf(" %d alimentos foram convertidos e salvos em %s\n", resultado, arquivo_saida);
        
        verificarArquivoBinario(arquivo_saida);
        
        FILE *arquivo = fopen(arquivo_saida, "rb");
        if (arquivo != NULL) {
            fseek(arquivo, 0, SEEK_END);
            long tamanho = ftell(arquivo);
            fclose(arquivo);
            printf("\n Tamanho do arquivo binário: %ld bytes\n", tamanho);
            printf(" Tamanho de cada registro: %zu bytes\n", sizeof(Alimento));
        }
        
    } else if (resultado == 0) {
        printf("  Nenhum dado foi encontrado no arquivo de entrada\n");
    } else {
        printf(" Erro durante a conversão\n");
        return 1;
    }
    
    return 0;
}