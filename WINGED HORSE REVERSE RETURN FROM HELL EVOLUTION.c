#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <math.h>

#define MAX_NIVEL 8

int totaldechecks=0;
int totalpodas=0;

typedef struct jogada{
    int linha,coluna;
}jogada;

typedef struct elemento{
    jogada jog;
    struct elemento *prox, *ant;
}elemento;

typedef struct posicao{
    int tabuleiro[8][8];
    int jogadorVez;
    int pecaPreta, pecaBranca;
}posicao;

posicao IniciaTabuleiro();
void DesenhaTabuleiro(posicao p);
int PrintaLista(elemento *lista);
jogada EscolheJogada(elemento *jogadas);
int PodeJogar(posicao tab,jogada jog, int dlin, int dcol);
elemento *CalculaJogadaValida(posicao p);
posicao Troca(posicao p,jogada *jv,int dlin,int dcol);
int ExecutaJogada(posicao *p, jogada j);
int ContaPecas(posicao p);
int ChecaVencedor(posicao p);
void Creditos();
double AvaliaPosicao(posicao p);
jogada ExecutaIA(posicao posAtual, int nivel, double alfa, double beta);
void DestruirLista(elemento **lista);
void SalvaJogada(char *arquivo,jogada *jog);

int main(){
    setlocale(LC_CTYPE,"Portuguese");

    jogada jgd;
    elemento *jogadas;
    char *arq="jogadas.bin";
    posicao tab = IniciaTabuleiro();
    int vencedor = 0;

    FILE *aqv=fopen(arq,"wb");
    fclose(aqv);

    SalvaJogada(arq,NULL);

    while(vencedor == 0){
        DesenhaTabuleiro(tab);

        if(tab.jogadorVez == -1){
            jogadas = CalculaJogadaValida(tab);
            jgd = EscolheJogada(jogadas);
            DestruirLista(&jogadas);
        }else{
            jgd = ExecutaIA(tab, 0, -INFINITY, INFINITY);
        }
        if(jgd.coluna == -1){
            printf("\nSem jogadas disponí­veis passando o turno...\n\n");
        }else{
            printf("\nJogada selecionada [%d][%d]", jgd.linha, jgd.coluna);
        }
        vencedor = ExecutaJogada(&tab,jgd);
        SalvaJogada(arq,&jgd);
        // printf("\nTotalChecks: %d\nTotalPodas: %d\n",totaldechecks,totalpodas);
        totaldechecks=0;
        totalpodas=0;
    }
    if(vencedor == -1){
        printf("\n\n>>>>Vencedor peça preta!<<<<\n");
        Creditos();
    }
    else if (vencedor == 1){
        printf("\n\n>>>>Vencedor peça branca!<<<<\n");
        Creditos();
    }
    else{
        printf("\n\n>>>>Empate!<<<<\n");
        Creditos();
    }

}
posicao IniciaTabuleiro(){
    posicao tab;

    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            tab.tabuleiro[i][j]=0;
        }
    }
    tab.tabuleiro[3][3] = tab.tabuleiro[4][4] = 1;
    tab.tabuleiro[3][4] = tab.tabuleiro[4][3] = -1;

    tab.pecaPreta = 2;
    tab.pecaBranca = 2;

    tab.jogadorVez = -1;

    return tab;
}
void DesenhaTabuleiro(posicao p){
    printf("\n   0  1  2  3  4  5  6  7\n");
    for(int linha = 0; linha < 8; linha++){
        for(int coluna = 0; coluna < 8; coluna++){
            if(coluna == 0)
                printf(" %d",linha);
            if(p.tabuleiro[linha][coluna] == 0)
                printf(" . ");
            else
                printf("%2d ",p.tabuleiro[linha][coluna]);
            if(coluna == 7)
                printf("%d",linha);
        }
        printf("\n");
    }
    printf("   0  1  2  3  4  5  6  7\n");
}
int PrintaLista(elemento *lista){
   elemento *aux = lista->prox;
   int k = 1;
   do{
        printf("\n%d:[%d][%d]", k, aux->jog.linha, aux->jog.coluna);
        aux = aux->prox;
        k++;
    }while(aux != lista);
    return k;
}
jogada EscolheJogada(elemento *jogadas){

   elemento *aux = jogadas->prox;
   if(aux == jogadas){
       jogada erro;
       erro.coluna = -1;
       return erro;
   }else{
    int k = PrintaLista(jogadas);
    int j;
    do{
        printf("\nQual jogada deseja executar? ");
        scanf("%d", &j);
        if(j>=k || j<=0) printf("\nDigite uma Jogada Valida");
    }while(j>=k || j<=0);

    k = 1;
    while(k < j){
        aux = aux->prox;
        k++;
    }

    jogada escolhida = aux->jog;

    return escolhida;
   }
}
int PodeJogar(posicao tab,jogada jog, int dlin, int dcol){
    int i = jog.linha+dlin;
    int j = jog.coluna+dcol;

   if(tab.tabuleiro[i][j]==tab.jogadorVez) return 0;

    while (i>=0 && j>=0 && i<8 && j<8 && tab.tabuleiro[i][j] == -tab.jogadorVez){
        i += dlin;
        j += dcol;
    }

    if (i<0||j<0||i>=8||j>=8) return 0;
    if (tab.tabuleiro[i][j]==0) return 0;
    return 1;
}
elemento *CalculaJogadaValida(posicao p){
    elemento *jogadasValidas = (elemento*) malloc(sizeof(elemento));
    jogadasValidas->prox = jogadasValidas;
    jogadasValidas->ant = jogadasValidas;

    for(int linha = 0; linha < 8; linha++){
        for(int coluna = 0; coluna < 8; coluna++){
            int valida = 0;
            jogada jog;
            jog.linha = linha;jog.coluna = coluna;
            if (jog.linha>=0&&jog.coluna>=0&&jog.linha<8&&jog.coluna<8&&p.tabuleiro[jog.linha][jog.coluna]==0){
                for (int deltaLinha=-1;deltaLinha<=1;deltaLinha++){
                    for (int deltaColuna=-1;deltaColuna<=1;deltaColuna++){
                        if (deltaLinha!=0||deltaColuna!=0){
                            if (PodeJogar(p,jog,deltaLinha,deltaColuna)==1){
                                valida++;
                            }
                        }
                    }
                }
                if(valida > 0){
                elemento *j = (elemento*) malloc(sizeof(elemento));
                j->jog = jog;
                j->prox = jogadasValidas;
                j->ant = jogadasValidas->ant;
                jogadasValidas->ant->prox = j;
                jogadasValidas->ant = j;
                }
            }
        }
    }
    return jogadasValidas;
}

posicao Troca(posicao p,jogada *jv,int dlin,int dcol){
    int i = jv->linha+dlin;
    int j = jv->coluna+dcol;

    if(p.tabuleiro[i][j]==-p.jogadorVez){
        while (p.tabuleiro[i][j]==-p.jogadorVez){
            p.tabuleiro[i][j] = -p.tabuleiro[i][j];

            i += dlin;
            j += dcol;
       }
   }
   return p;
}
int ContaPecas(posicao p){
    if(p.pecaPreta > p.pecaBranca)
        return -1;
    else if(p.pecaBranca > p.pecaPreta)
        return 1;
    else
        return 2;
}
int ChecaVencedor(posicao p){
    if(p.pecaBranca + p.pecaPreta == 64){
        return ContaPecas(p);
    }
    elemento *lista;
    lista = CalculaJogadaValida(p);
    if(lista->prox == lista){
        DestruirLista(&lista);
        p.jogadorVez = -p.jogadorVez;
        lista = CalculaJogadaValida(p);
        if(lista->prox == lista){
            DestruirLista(&lista);
            return ContaPecas(p);
        }
    }
    return 0;
}
int ExecutaJogada(posicao *p, jogada j){
    for (int lin=-1;lin<=1&&j.coluna!=-1;lin++){
        for (int col=-1;col<=1;col++){
            if (lin!=0||col!=0){
               int w=PodeJogar(*p,j,lin,col);
               if(w==1){
                      *p = Troca(*p,&j,lin,col);

                      p->tabuleiro[j.linha][j.coluna] = p->jogadorVez;
                }
            }
        }
    }
    int pB, pP;
    pB = pP = 0;
    for(int linha = 0; linha < 8; linha++){
        for(int coluna = 0; coluna < 8; coluna++){
            if(p->tabuleiro[linha][coluna] == -1)
                pP++;
            if(p->tabuleiro[linha][coluna] == 1)
                pB++;
        }
    }
    p->pecaBranca = pB;
    p->pecaPreta = pP;
    p->jogadorVez = -p->jogadorVez;
    return ChecaVencedor(*p);
}
void DestruirLista(elemento **lista){
    struct elemento *aux = *lista;
    aux->ant->prox = NULL;
    do{
        *lista = aux->prox;
        free(aux);
        aux = *lista;
    }while(aux != NULL);
}

void Creditos(){
    printf("\n\n          ## Parabens voce Desbloqueou o Final Bom( Em que o jogo nao buga e fecha repentinamente ;) ) ##\n\n");
    printf("\n\n               | Muito Obrigado por jogar o WINGED HORSE REVERSI: RETURN FROM HELL EVOLUTION |\n\n");
    printf("\n\n                Developer by: Rafael O Mais Brabo && Silva da Silva && Pastel cara de Raquel\n\n");
}
double AvaliaPosicao(posicao p){
    double avaliacao = 0;

    for(int linha = 0; linha < 8; linha++){
        for(int coluna = 0; coluna < 8; coluna++){
            if(p.tabuleiro[linha][coluna] == -1){
                avaliacao-=abs( (4-linha) + (3-coluna) );
            }else if(p.tabuleiro[linha][coluna] == 1){
                avaliacao+=abs( (4-linha) + (3-coluna) );
            }
        }
    }
    return avaliacao;
}

jogada ExecutaIA(posicao posAtual, int nivel, double alfa, double beta){
    jogada melhorJogada, jogadaIA;
    elemento *jogadaAux, *lista;
    posicao posCopia=posAtual;

    int podado = 0;
    double melhorValor, valorJogada;

    if(nivel % 2 == 0){
        posCopia.jogadorVez = 1;
        melhorValor = -INFINITY;
    }
    else{
        posCopia.jogadorVez = -1;
        melhorValor = INFINITY;
    }

    lista = CalculaJogadaValida(posCopia);

    jogadaAux = lista->prox;

    while(jogadaAux != lista && podado == 0){

        posCopia = posAtual;
        ExecutaJogada(&posCopia, jogadaAux->jog);

        if(nivel < MAX_NIVEL){
            if(nivel % 2 == 0){
                jogadaIA = ExecutaIA(posCopia, nivel + 1, melhorValor, beta);
            }else{
                jogadaIA = ExecutaIA(posCopia, nivel + 1, alfa, melhorValor);
            }
            ExecutaJogada(&posCopia, jogadaIA);
        }else jogadaIA=jogadaAux->jog;

        valorJogada = AvaliaPosicao(posCopia);

        if(valorJogada < alfa || valorJogada > beta){
            podado = 1;
            totalpodas++;
        }

        if(nivel % 2 == 0 && valorJogada >= melhorValor){
            melhorValor = valorJogada;
            melhorJogada = jogadaIA;
        }
        else if(nivel % 2 != 0 && valorJogada <= melhorValor){
            melhorValor = valorJogada;
            melhorJogada = jogadaIA;
        }
      
        jogadaAux = jogadaAux->prox;
    }

    if(jogadaAux==lista && nivel<MAX_NIVEL) melhorJogada=jogadaAux->ant->jog;
    totaldechecks++;
    return melhorJogada;
}

void SalvaJogada(char *arquivo,jogada *jog){
    FILE *arq=fopen(arquivo,"ab");

    if(jog != NULL) fwrite(jog,sizeof(int),2,arq);

    fclose(arq);
}
