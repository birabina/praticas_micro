#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

/* CONFIGURACOES DO SISTEMA VIA MACROS */
#define PORTA_SERIAL        "/dev/ttyACM0"
#define BAUD_RATE           B115200
#define ARQUIVO_PERFIS      "perfis_jogadores.txt"

/* LIMITES E DIMENSOES */
#define MAX_NICK            50
#define MAX_PERFIS          100
#define BUFFER_SERIAL       32
#define TOTAL_COMANDOS      3
#define LARGURA_TELA        55

/* ESTADOS DE RETORNO */
#define STATUS_SUCESSO      1
#define STATUS_FALHA        0
#define MODO_SIMULACAO      -1

/* MACROS DE PROTOCOLO JOGO <-> STM32 */
#define FORMATO_TREINO(buf, id)  sprintf(buf, "T%d\n", id)
#define FORMATO_CARGA(buf, id)   sprintf(buf, "L%d\n", id)
#define FORMATO_OK(buf, id)      sprintf(buf, "OK%d", id)

/* CONFIGURACAO DE ESTILOS E CORES ANSI (TEMA ROXO) */
#define ANSI_RESET            "\033[0m"
#define ANSI_NEGRITO          "\033[1m"
#define ANSI_DIM              "\033[2m"

/* CORES DE TEXTO SEMANTICAS */
#define COR_PRINCIPAL         "\033[35m"  /* Roxo / Magenta */
#define COR_DESTAQUE          "\033[95m"  /* Roxo Claro / Lilas */
#define COR_SUCESSO           "\033[32m"  /* Verde */
#define COR_ALERTA            "\033[33m"  /* Amarelo */
#define COR_ERRO              "\033[31m"  /* Vermelho */
#define COR_TEXTO_MUTED       "\033[90m"  /* Cinza Escuro */

/* CARACTERES DE BORDA (ASCII ESTENDIDA) */
#define BORDA_HORIZ           "═"
#define BORDA_VERT            "║"
#define CANTO_SUP_ESQ         "╔"
#define CANTO_SUP_DIR         "╗"
#define CANTO_INF_ESQ         "╚"
#define CANTO_INF_DIR         "╝"
#define DIVISOR_ESQ           "╠"
#define DIVISOR_DIR           "╣"

static const char *COMANDOS_VOZ[TOTAL_COMANDOS] = {"PULAR", "ATIRAR", "AGACHAR"};

void limpar_tela() {
    printf("\033[H\033[J");
}

void desenhar_cabecalho(const char *titulo) {
    int tam_titulo = strlen(titulo);
    int espacos_totais = LARGURA_TELA - tam_titulo - 4; 
    int espacos_esq = espacos_totais / 2;
    int espacos_dir = espacos_totais - espacos_esq;

    limpar_tela();

    printf(COR_PRINCIPAL "%s", CANTO_SUP_ESQ);
    for (int i = 0; i < LARGURA_TELA - 2; i++) printf("%s", BORDA_HORIZ);
    printf("%s\n" ANSI_RESET, CANTO_SUP_DIR);

    printf(COR_PRINCIPAL "%s " ANSI_RESET, BORDA_VERT);
    for (int i = 0; i < espacos_esq; i++) printf(" ");
    printf(ANSI_NEGRITO COR_DESTAQUE "%s" ANSI_RESET, titulo);
    for (int i = 0; i < espacos_dir; i++) printf(" ");
    printf(COR_PRINCIPAL " %s\n" ANSI_RESET, BORDA_VERT);

    printf(COR_PRINCIPAL "%s", DIVISOR_ESQ);
    for (int i = 0; i < LARGURA_TELA - 2; i++) printf("%s", BORDA_HORIZ);
    printf("%s\n" ANSI_RESET, DIVISOR_DIR);
}

void desenhar_rodape() {
    printf(COR_PRINCIPAL "%s", CANTO_INF_ESQ);
    for (int i = 0; i < LARGURA_TELA - 2; i++) printf("%s", BORDA_HORIZ);
    printf("%s\n" ANSI_RESET, CANTO_INF_DIR);
}

int configurar_serial() {
    int fd = open(PORTA_SERIAL, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        printf(COR_ERRO "[ERRO]" ANSI_RESET " Nao foi possivel abrir a porta %s: %s\n", PORTA_SERIAL, strerror(errno));
        printf(COR_TEXTO_MUTED "[INFO] Iniciando em modo de simulacao (sem hardware)...\n" ANSI_RESET);
        sleep(2);
        return MODO_SIMULACAO;
    }

    fcntl(fd, F_SETFL, 0);

    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    options.c_cc[VMIN] = 0;   
    options.c_cc[VTIME] = 10; 

    tcsetattr(fd, TCSANOW, &options);
    printf(COR_SUCESSO "[SISTEMA]" ANSI_RESET " Conectado a STM32 em %s\n", PORTA_SERIAL);
    sleep(1);
    return fd;
}

int aguardar_confirmacao_stm32(int fd, int id_esperado) {
    char comando_ok[BUFFER_SERIAL];
    FORMATO_OK(comando_ok, id_esperado);
    
    printf(COR_ALERTA "[VR3]" ANSI_RESET " Aguardando comando de voz... Fale agora.\n");

    if (fd == MODO_SIMULACAO) {
        printf(COR_TEXTO_MUTED "      (Modo Simulacao: Pressione ENTER para simular resposta)\n" ANSI_RESET);
        getchar(); 
        getchar(); 
        return STATUS_SUCESSO;
    }

    char buffer[BUFFER_SERIAL];
    char caractere;
    int index = 0;

    tcflush(fd, TCIOFLUSH);

    while (1) {
        int n = read(fd, &caractere, 1);
        if (n > 0) {
            if (caractere == '\n' || caractere == '\r') {
                if (index > 0) {
                    buffer[index] = '\0';
                    
                    if (strcmp(buffer, comando_ok) == 0) {
                        printf(COR_SUCESSO "[STM32]" ANSI_RESET " Confirmado: %s\n", buffer);
                        return STATUS_SUCESSO;
                    } else if (strncmp(buffer, "ERRO", 4) == 0) {
                        printf(COR_ERRO "[STM32]" ANSI_RESET " Falha no treino: %s. Revezando...\n", buffer);
                        return STATUS_FALHA;
                    }
                    index = 0; 
                }
            } else {
                if (index < (BUFFER_SERIAL - 1)) {
                    buffer[index++] = caractere;
                }
            }
        }
    }
}

void tela_cadastro(int fd) {
    char nick[MAX_NICK];
    desenhar_cabecalho("NOVO CADASTRO DE JOGADOR");
    
    printf(" Digite o seu Nick: ");
    scanf("%s", nick);

    FILE *arquivo = fopen(ARQUIVO_PERFIS, "a");
    if (arquivo != NULL) {
        fprintf(arquivo, "%s\n", nick);
        fclose(arquivo);
        printf("\n" COR_SUCESSO "[GRAVACAO]" ANSI_RESET " Perfil '%s' salvo localmente.\n", nick);
    } else {
        printf("\n" COR_ERRO "[ERRO]" ANSI_RESET " Falha ao persistir dados localmente.\n");
    }
    sleep(1);

    for (int id = 0; id < TOTAL_COMANDOS; id++) {
        char titulo_treino[MAX_NICK + 30];
        sprintf(titulo_treino, "TREINO DE VOZ - JOGADOR: %s", nick);
        desenhar_cabecalho(titulo_treino);

        printf(" Comando atual: " ANSI_NEGRITO COR_DESTAQUE "[%s]" ANSI_RESET "\n", COMANDOS_VOZ[id]);
        printf(COR_TEXTO_MUTED " -----------------------------------------------------\n" ANSI_RESET);
        printf("[INFO] Prepare-se para falar quando o VR3 ativar.\n\n");
        sleep(1);

        char string_envio[BUFFER_SERIAL];
        FORMATO_TREINO(string_envio, id);
        printf(COR_ALERTA "[UART]" ANSI_RESET " Enviando solicitacao: %s", string_envio);

        if (fd != MODO_SIMULACAO) {
            write(fd, string_envio, strlen(string_envio));
        }

        int sucesso = STATUS_FALHA;
        while (!sucesso) {
            sucesso = aguardar_confirmacao_stm32(fd, id);
        }

        printf("\n" COR_SUCESSO "[SUCESSO]" ANSI_RESET " Comando '%s' gravado com sucesso!\n", COMANDOS_VOZ[id]);
        sleep(2);
    }
}

void tela_perfis(int fd) {
    desenhar_cabecalho("PERFIS REGISTRADOS NO COMPUTADOR");

    FILE *arquivo = fopen(ARQUIVO_PERFIS, "r");
    if (arquivo == NULL) {
        printf(" [INFO] Nenhum jogador cadastrado ate o momento.\n");
        desenhar_rodape();
        printf("\nPressione ENTER para retornar...");
        getchar(); getchar();
        return;
    }

    char perfis[MAX_PERFIS][MAX_NICK];
    int total_perfis = 0;

    while (fscanf(arquivo, "%s", perfis[total_perfis]) != EOF && total_perfis < MAX_PERFIS) {
        printf("  " COR_PRINCIPAL "%s" ANSI_RESET " [%d] %s\n", BORDA_VERT, total_perfis, perfis[total_perfis]);
        total_perfis++;
    }
    fclose(arquivo);

    printf(COR_TEXTO_MUTED " ╠═════════════════════════════════════════════════════\n" ANSI_RESET);
    printf("  [A] Carregar Perfil para Jogo\n");
    printf("  [D] Deletar um Perfil Antigo\n");
    printf("  [V] Voltar ao Menu Principal\n");
    printf(COR_TEXTO_MUTED " ╠═════════════════════════════════════════════════════\n" ANSI_RESET);
    printf("  Escolha a acao (A/D/V): ");
    char acao;
    scanf(" %c", &acao);

    if (acao == 'V' || acao == 'v') return;

    if (acao == 'A' || acao == 'a') {
        printf("  Digite o numero do perfil para carregar: ");
        int escolha;
        scanf("%d", &escolha);

        if (escolha >= 0 && escolha < total_perfis) {
            int id_inicial = escolha * TOTAL_COMANDOS;
            char comando_carga[BUFFER_SERIAL];
            FORMATO_CARGA(comando_carga, id_inicial);

            printf("\n [PERFIL] Selecionado de forma ativa: " ANSI_NEGRITO "%s" ANSI_RESET "\n", perfis[escolha]);
            printf(COR_ALERTA "[UART]" ANSI_RESET " Enviando comando de carga: %s", comando_carga);

            if (fd != MODO_SIMULACAO) {
                write(fd, comando_carga, strlen(comando_carga));
            }

            printf("\n" COR_SUCESSO "[OK]" ANSI_RESET " Perfil carregado na memória flash!\n");
            printf("\nPressione ENTER para encerrar a simulacao de partida...");
            getchar(); getchar();
        } else {
            printf("\n" COR_ERRO "[ERRO]" ANSI_RESET " Opcao invalida.\n");
            sleep(1);
        }
    } 
    else if (acao == 'D' || acao == 'd') {
        printf("  Digite o numero do perfil que deseja DELETAR: ");
        int escolha;
        scanf("%d", &escolha);

        if (escolha >= 0 && escolha < total_perfis) {
            /* Sobrescreve o arquivo gravando todos, EXCETO o escolhido */
            arquivo = fopen(ARQUIVO_PERFIS, "w");
            if (arquivo != NULL) {
                for (int i = 0; i < total_perfis; i++) {
                    if (i != escolha) {
                        fprintf(arquivo, "%s\n", perfis[i]);
                    }
                }
                fclose(arquivo);
                printf("\n" COR_SUCESSO "[DELETADO]" ANSI_RESET " Perfil '%s' removido com sucesso.\n", perfis[escolha]);
            } else {
                printf("\n" COR_ERRO "[ERRO]" ANSI_RESET " Falha ao abrir arquivo para remocao.\n");
            }
            sleep(2);
        } else {
            printf("\n" COR_ERRO "[ERRO]" ANSI_RESET " Indice invalida.\n");
            sleep(1);
        }
    }
}

void tela_sobre() {
    desenhar_cabecalho("SOBRE O PROJETO / ACESSIBILIDADE");
    printf("  Objetivo:\n");
    printf("    Demonstrar acessibilidade e controle por voz em jogos\n");
    printf("    utilizando a plataforma ARM STM32 e C ANSI nativo.\n\n");
    printf("  Funcionamento do Modulo VR3:\n");
    printf("    O hardware mapeia e compara o espectro de frequencia\n");
    printf("    da onda acustica. Nao ha interpretacao gramatical,\n");
    printf("    apenas casamento de padroes de onda senoidais.\n");
    desenhar_rodape();
    printf("\nPressione ENTER para retornar ao Menu Principal...");
    getchar(); getchar();
}

int main() {
    int fd_serial = configurar_serial();
    int opcao;

    while (1) {
        desenhar_cabecalho("MENU PRINCIPAL - EVOKE");
        
        printf(COR_PRINCIPAL "  %s  " ANSI_NEGRITO "[1]" ANSI_RESET " Novo Cadastro (Treinar Voz)\n", BORDA_VERT);
        printf(COR_PRINCIPAL "  %s  " ANSI_NEGRITO "[2]" ANSI_RESET " Gerenciar Perfis (Carregar/Deletar)\n", BORDA_VERT);
        printf(COR_PRINCIPAL "  %s  " ANSI_NEGRITO "[3]" ANSI_RESET " Sobre o Projeto\n", BORDA_VERT);
        printf(COR_PRINCIPAL "  %s  " COR_ERRO     "[4]" ANSI_RESET " Sair do Sistema\n", BORDA_VERT);
        
        desenhar_rodape();
        
        printf(ANSI_NEGRITO COR_PRINCIPAL "  » " ANSI_RESET "Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            continue;
        }

        switch (opcao) {
            case 1:
                tela_cadastro(fd_serial);
                break;
            case 2:
                tela_perfis(fd_serial);
                break;
            case 3:
                tela_sobre();
                break;
            case 4:
                limpar_tela();
                printf(COR_TEXTO_MUTED "[INFO] Finalizando execucao do terminal de controle.\n" ANSI_RESET);
                if (fd_serial != MODO_SIMULACAO) {
                    close(fd_serial);
                }
                return 0;
            default:
                printf("\n" COR_ERRO "[ERRO]" ANSI_RESET " Opcao invalida.\n");
                sleep(1);
        }
    }
    return 0;
}
