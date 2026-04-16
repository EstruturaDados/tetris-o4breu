#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ================================================
   GERENCIADOR DE PEÇAS — FILA CIRCULAR + PILHA
   Fila: capacidade 5 | Pilha: capacidade 3
   ================================================ */

#define TAM_FILA  5
#define TAM_PILHA 3

/* --- Struct Peca: representa cada peça do jogo --- */
typedef struct {
    char nome; /* Tipo: 'I', 'O', 'T' ou 'L'       */
    int  id;   /* Identificador único de criação     */
} Peca;

/* --- Fila Circular --- */
typedef struct {
    Peca dados[TAM_FILA];
    int  inicio;  /* Índice da frente da fila         */
    int  fim;     /* Índice do próximo espaço livre   */
    int  tamanho; /* Quantidade atual de elementos    */
} FilaCircular;

/* --- Pilha Linear --- */
typedef struct {
    Peca dados[TAM_PILHA];
    int  topo; /* -1 = vazia                         */
} Pilha;

/* Contador global de IDs (incrementa a cada peça criada) */
int proximoId = 0;

/* ================================================
   FUNÇÃO: gerarPeca
   Cria uma peça com nome aleatório e ID único.
   ================================================ */
Peca gerarPeca() {
    char tipos[] = {'I', 'O', 'T', 'L'};
    Peca p;
    p.nome = tipos[rand() % 4];
    p.id   = proximoId++;
    return p;
}

/* ================================================
   OPERAÇÕES DA FILA CIRCULAR
   ================================================ */

/* Inicializa a fila como vazia */
void inicializarFila(FilaCircular *f) {
    f->inicio  = 0;
    f->fim     = 0;
    f->tamanho = 0;
}

int filaCheia(FilaCircular *f)  { return f->tamanho == TAM_FILA; }
int filaVazia(FilaCircular *f)  { return f->tamanho == 0; }

/* Insere peça no fim da fila circular */
int enfileirar(FilaCircular *f, Peca p) {
    if (filaCheia(f)) return 0;
    f->dados[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA; /* Avança circularmente */
    f->tamanho++;
    return 1;
}

/* Remove e retorna a peça da frente */
Peca desenfileirar(FilaCircular *f, int *ok) {
    Peca vazia = {' ', -1};
    if (filaVazia(f)) { *ok = 0; return vazia; }
    Peca p = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->tamanho--;
    *ok = 1;
    return p;
}

/* Retorna (sem remover) a peça da frente */
Peca frente(FilaCircular *f, int *ok) {
    Peca vazia = {' ', -1};
    if (filaVazia(f)) { *ok = 0; return vazia; }
    *ok = 1;
    return f->dados[f->inicio];
}

/* ================================================
   OPERAÇÕES DA PILHA LINEAR
   ================================================ */

void inicializarPilha(Pilha *p) { p->topo = -1; }

int pilhaCheia(Pilha *p)  { return p->topo == TAM_PILHA - 1; }
int pilhaVazia(Pilha *p)  { return p->topo == -1; }

int empilhar(Pilha *p, Peca pc) {
    if (pilhaCheia(p)) return 0;
    p->dados[++p->topo] = pc;
    return 1;
}

Peca desempilhar(Pilha *p, int *ok) {
    Peca vazia = {' ', -1};
    if (pilhaVazia(p)) { *ok = 0; return vazia; }
    *ok = 1;
    return p->dados[p->topo--];
}

/* ================================================
   FUNÇÃO: exibirEstado
   Mostra a fila e a pilha formatadas.
   ================================================ */
void exibirEstado(FilaCircular *f, Pilha *p) {
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║              ESTADO ATUAL                    ║\n");
    printf("╠══════════════════════════════════════════════╣\n");

    /* Exibe fila da frente ao fim */
    printf("║ Fila de peças    : ");
    if (filaVazia(f)) {
        printf("[ vazia ]");
    } else {
        for (int i = 0; i < f->tamanho; i++) {
            int idx = (f->inicio + i) % TAM_FILA;
            printf("[%c %d] ", f->dados[idx].nome, f->dados[idx].id);
        }
    }
    printf("\n");

    /* Exibe pilha do topo à base */
    printf("║ Pilha de reserva : (Topo -> base): ");
    if (pilhaVazia(p)) {
        printf("[ vazia ]");
    } else {
        for (int i = p->topo; i >= 0; i--)
            printf("[%c %d] ", p->dados[i].nome, p->dados[i].id);
    }
    printf("\n");
    printf("╚══════════════════════════════════════════════╝\n");
}

/* Reabastece a fila após remoção, gerando nova peça automaticamente */
void reabastecer(FilaCircular *f) {
    if (!filaCheia(f)) {
        Peca nova = gerarPeca();
        enfileirar(f, nova);
        printf("  ♻ Nova peça gerada automaticamente: [%c %d]\n",
               nova.nome, nova.id);
    }
}

/* ================================================
   AÇÕES DO JOGO
   ================================================ */

/* Ação 1: Jogar peça da frente da fila (descartá-la) */
void jogarPeca(FilaCircular *f) {
    int ok;
    Peca p = desenfileirar(f, &ok);
    if (!ok) { printf("⚠ Fila vazia! Nenhuma peça para jogar.\n"); return; }
    printf("  ▶ Peça jogada: [%c %d]\n", p.nome, p.id);
    reabastecer(f);
}

/* Ação 2: Mover peça da frente da fila para a pilha */
void reservarPeca(FilaCircular *f, Pilha *p) {
    if (pilhaCheia(p)) { printf("⚠ Pilha cheia! Não é possível reservar.\n"); return; }
    int ok;
    Peca pc = desenfileirar(f, &ok);
    if (!ok) { printf("⚠ Fila vazia!\n"); return; }
    empilhar(p, pc);
    printf("  📦 Reservada: [%c %d] → pilha\n", pc.nome, pc.id);
    reabastecer(f);
}

/* Ação 3: Usar (descartar) peça do topo da pilha */
void usarReservada(Pilha *p) {
    int ok;
    Peca pc = desempilhar(p, &ok);
    if (!ok) { printf("⚠ Pilha vazia! Nenhuma peça reservada.\n"); return; }
    printf("  ✅ Peça reservada usada: [%c %d]\n", pc.nome, pc.id);
}

/* Ação 4: Trocar a frente da fila com o topo da pilha */
void trocarUma(FilaCircular *f, Pilha *p) {
    if (filaVazia(f))  { printf("⚠ Fila vazia!\n");  return; }
    if (pilhaVazia(p)) { printf("⚠ Pilha vazia!\n"); return; }

    /* Troca direta: remove de ambos e reinserem invertidos */
    int ok;
    Peca dFila  = desenfileirar(f, &ok);
    Peca dPilha = desempilhar(p, &ok);

    /* Reinsere: fila recebe o da pilha na frente (enfileirar normal),
       pilha recebe o da fila */
    /* Para inserir na frente da fila circular, ajustamos o início */
    f->inicio  = (f->inicio - 1 + TAM_FILA) % TAM_FILA;
    f->dados[f->inicio] = dPilha;
    f->tamanho++;

    empilhar(p, dFila);

    printf("  🔄 Troca realizada: fila←[%c %d] | pilha←[%c %d]\n",
           dPilha.nome, dPilha.id, dFila.nome, dFila.id);
}

/* Ação 5: Trocar as 3 primeiras da fila com as 3 da pilha */
void trocarMultipla(FilaCircular *f, Pilha *p) {
    if (f->tamanho < 3) {
        printf("⚠ A fila precisa ter pelo menos 3 peças!\n"); return;
    }
    if (p->topo < 2) {
        printf("⚠ A pilha precisa ter pelo menos 3 peças!\n"); return;
    }

    /* Salva as 3 primeiras da fila */
    Peca dFila[3];
    int ok;
    for (int i = 0; i < 3; i++)
        dFila[i] = desenfileirar(f, &ok);

    /* Salva as 3 da pilha (topo → base) */
    Peca dPilha[3];
    for (int i = 0; i < 3; i++)
        dPilha[i] = desempilhar(p, &ok);

    /* Coloca peças da pilha na frente da fila (ordem: [2],[1],[0] no início) */
    for (int i = 0; i < 3; i++) {
        f->inicio  = (f->inicio - 1 + TAM_FILA) % TAM_FILA;
        f->dados[f->inicio] = dPilha[2 - i];
        f->tamanho++;
    }

    /* Coloca peças da fila na pilha (base → topo: [2],[1],[0]) */
    for (int i = 2; i >= 0; i--)
        empilhar(p, dFila[i]);

    printf("  🔀 Troca múltipla realizada entre os 3 primeiros da fila e os 3 da pilha.\n");
}

/* ================================================
   MAIN
   ================================================ */
int main() {
    srand((unsigned)time(NULL));

    FilaCircular fila;
    Pilha        pilha;

    inicializarFila(&fila);
    inicializarPilha(&pilha);

    /* Preenche a fila com 5 peças iniciais */
    printf("🎮 Inicializando peças...\n");
    for (int i = 0; i < TAM_FILA; i++)
        enfileirar(&fila, gerarPeca());

    int opcao;

    do {
        exibirEstado(&fila, &pilha);

        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║              OPÇÕES DISPONÍVEIS              ║\n");
        printf("╠═══╦══════════════════════════════════════════╣\n");
        printf("║ 1 ║ Jogar peça da frente da fila             ║\n");
        printf("║ 2 ║ Enviar peça da fila para a pilha         ║\n");
        printf("║ 3 ║ Usar peça da pilha de reserva            ║\n");
        printf("║ 4 ║ Trocar frente da fila com topo da pilha  ║\n");
        printf("║ 5 ║ Trocar os 3 primeiros (fila ↔ pilha)     ║\n");
        printf("║ 0 ║ Sair                                     ║\n");
        printf("╚═══╩══════════════════════════════════════════╝\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        printf("\n");

        switch (opcao) {
            case 1: jogarPeca(&fila);           break;
            case 2: reservarPeca(&fila, &pilha); break;
            case 3: usarReservada(&pilha);       break;
            case 4: trocarUma(&fila, &pilha);    break;
            case 5: trocarMultipla(&fila, &pilha); break;
            case 0: printf("🔒 Encerrando o jogo. Até a próxima!\n\n"); break;
            default: printf("⚠ Opção inválida!\n");
        }

    } while (opcao != 0);

    return 0;
}
