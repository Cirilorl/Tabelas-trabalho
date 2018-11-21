//não excluir colunas das tabelas do excel nem nada do tipo!
//arrumar tabelas csv de forma que não contenham ; desnecessários(importante)!
//o foco é entender o que acontece na main e a função menor de menor algoritmo
//tudo relacionado ao grafo(aquelas funções grandes), não precisa olhar
//tentei deixar a main fácil de ser entendida, basicamente abrindo arquivos, lendo e no final chamando a função de menor caminho
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define INFINITO 99999999 //definições do algoritmo de menor caminho
#define MEMBRO 1
#define NAO_MEMBRO 0

typedef struct s_nos{  
  int V;
}tnos;

typedef struct s_arcos{ 
  int adj;
  char nome[50];
  int tamanho;
}tarcos;

typedef struct s_grafo{ // estrutura do grafo
    tnos *nos;
    tarcos **arcos;
}tgrafo;

typedef struct s_lugares{ //estrutura de dos lugares
    char nome[50];
    int vertice_prox;
    int dist_vertice;
}tlugares;

enum op{SEM_ESCOLHA, ARESTA, LUGARES}; //isso escolhe se a função abre_arquivo vai abrir uma tabela de arestas ou lugares

int descobre_linhas(FILE *fp);
void abre_arquivo(FILE *fp, tarcos **arco, tlugares *lugares, int qtd_linhas, int arq_op);
void leitura_aresta(FILE *fp, char *s, tarcos **arco, int qtd_vert);
void printa_aresta(tarcos **arco, int qtd_vert);
void cria_grafo(tnos *no, tarcos **arco, tgrafo *grafo, int qtd_vert);
void leitura_lugares(FILE *fp, char *s, tlugares *lugares, int qtd_linhas);
void escolhe_vertice(tlugares *lugares, int qtd_linhas, int *v_inicial, int *v_final);
void menorCaminho(tlugares *lugares, tgrafo *grafo, int s, int t, int qtd_vert, int qtd_linhas);

int main(int argC, char *argV[]) {
  int i = 0, j = 0, parametros = 0,qtd_vert = 0, numero, teste = 0, linhas_lugares = 0, vi, vf;
  enum op arq_op = SEM_ESCOLHA;
  char ch;
  FILE *fp = NULL;
  tnos *nos;
  tarcos **arcos;
  tgrafo g;
  tlugares *lugares;

  // if(argC == 1) {
  //  printf("Parametros insuficientes...\nPasse o nome do arquivo\n");
  //  exit(1);
  // }

  if((fp = fopen("Vertices.csv","r")) == NULL) {    //teste com arquivo vertice
    printf("Nao foi possivel abrir o arquivo...");
    exit(1);
  }

/*  for (i=1; i<argC; i++) {
    parametros++; // quantidade de parametros
  }*/

  qtd_vert = descobre_linhas(fp);
  nos = (tnos *)calloc(qtd_vert,sizeof(tnos));
  for(i = 0; i < qtd_vert; i++){
      nos[i].V = (i + 1);
  }
  fclose(fp);

  if((fp = fopen("Ciclovias_e_Parques.csv","r")) == NULL) {    //abre arquivo de ciclovia
    printf("Nao foi possivel abrir o arquivo...");
    exit(1);
  }
  arcos = (tarcos **)calloc(qtd_vert, sizeof(tarcos *));       //alocação da matriz
  for(i = 0; i < qtd_vert; i++){
    arcos[i] = (tarcos *)calloc(qtd_vert, sizeof(tarcos));
  }
  arq_op = ARESTA;
  abre_arquivo(fp, arcos, lugares, qtd_vert, arq_op);       //faz a leitura do arquivo
  //printa_aresta(arcos, qtd_vert);
  cria_grafo(nos, arcos, &g, qtd_vert);

  if((fp = fopen("Parques.csv","r")) == NULL){
        printf("Nao foi possivel abrir o arquivo...");
        exit(1);
  }
  linhas_lugares = descobre_linhas(fp);
  lugares = (tlugares *)calloc(linhas_lugares, sizeof(tlugares)); 
  arq_op = LUGARES;
  abre_arquivo(fp, arcos, lugares, linhas_lugares, arq_op); //abre arquivo de lugares
  escolhe_vertice(lugares, linhas_lugares, &vi, &vf);  //
  printf("um:%d e dois: %d\n\n", vi, vf); //um teste
  system("pause");
  menorCaminho(lugares, &g, vi, vf, qtd_vert, linhas_lugares);
  free(nos);  // liberação da memória
  for(i = 0; i < qtd_vert; i++){
    free(arcos[i]);
  }
  free(arcos);
  free(lugares); //
  return 0;
}

int descobre_linhas(FILE *fp){
    char ch;
    int qtd_linhas = 0;
    while((ch = fgetc(fp)) != EOF){
    if(ch == 10){
      qtd_linhas++;
    }
  }
  qtd_linhas = qtd_linhas - 1;
  rewind(fp);
  return qtd_linhas;
}

void abre_arquivo(FILE *fp, tarcos **arco, tlugares *lugares, int qtd_linhas, int arq_op){
  char ch;
  int n = 1,j=0,i=0, colunas = 1, linha = 1, onde_estive, onde_estou, posicao = 1;
  char *p, *s;

  while((ch = fgetc(fp)) != 10){
    if(ch == ';'){
      colunas++;
    }               //pula a primeira linha
  }
  onde_estive = ftell(fp);

  while((ch = fgetc(fp)) != EOF){
      while((ch = fgetc(fp)) != 10){
           posicao++;
      }
      onde_estou = ftell(fp);
      fseek(fp, -(posicao+2), SEEK_CUR);
      s = (char *)malloc(posicao+2);
      fgets(s, (posicao+2), fp);
      //printf("\n\nteste sem strok %d:%s\n",linha, s);
      //printf("colunas do arquivo: %d\n", colunas);
      if(arq_op == ARESTA){
         leitura_aresta(fp, s, arco, qtd_linhas);
      }
      if(arq_op == LUGARES){
         leitura_lugares(fp, s, lugares, qtd_linhas);
      }
      free(s);
      posicao = 1;
      linha++;
      fseek(fp, (onde_estou), SEEK_SET);
  }
    fclose(fp);
}

void leitura_aresta(FILE *fp, char *s, tarcos **arco, int qtd_vert){
    int i, j, peso, n = 1;
    char nome_aresta[50], *p;

    p = strtok(s,";");
      while (p != NULL && n <= 9){
        //printf ("test:%s\n",p);
        if(n == 2){
            strcpy(nome_aresta, p);
        }
        if(n == 7){
            peso = atoi(p);
        }
        if(n == 8){
            i = atoi(p);
        }
        if(n == 9){
           j = atoi(p);
        }
        p = strtok (NULL, ";");
        n++;
      }
    arco[i-1][j-1].adj = 1;
    arco[j-1][i-1].adj = 1;
    arco[i-1][j-1].tamanho = peso;
    arco[j-1][i-1].tamanho = peso;
    strcpy(arco[i-1][j-1].nome,nome_aresta);
    strcpy(arco[j-1][i-1].nome,nome_aresta);
    n=1;

}

// void printa_aresta(tarcos **arco, int qtd_vert){
//      int l, c;
//      printf("Escolha linha:\n");
//      scanf("%d", &l);
//      printf("Escolha coluna:\n");
//      scanf("%d", &c);
//      printf("\tarco[%d][%d]: %d\n\ttamanho: %d\n\tnome: %s", l, c, arco[l-1][c-1].adj, arco[l-1][c-1].tamanho, arco[l-1][c-1].nome);
// }

void cria_grafo(tnos *no, tarcos **arco, tgrafo *grafo, int qtd_vert){
        int i, j;
        grafo->arcos = (tarcos**)malloc(qtd_vert*sizeof(tarcos*));
        for(i = 0; i < qtd_vert; i++){
            grafo->arcos[i] = (tarcos*)malloc(qtd_vert*sizeof(tarcos));
        }
        grafo->nos = (tnos*)malloc(qtd_vert*sizeof(tnos));
        grafo->arcos = arco;
        for(i = 0; i < qtd_vert; i++){
            for(j = 0; j < qtd_vert; j++){
                if(grafo->arcos[i][j].adj == 0){
                     grafo->arcos[i][j].tamanho = INFINITO;
                }
            }
        }
        grafo->nos = no;
}

void leitura_lugares(FILE *fp, char *s, tlugares *lugares, int qtd_linhas){
    int i = 0, vertice, n = 1, distancia;
    char nome_aresta[50], *p;

    p = strtok(s,";");
      while (p != NULL && n <= 6){
        //printf ("test:%s\n",p);
        if(n == 1){
            i = atoi(p);
        }
        if(n == 2){
            strcpy(nome_aresta, p);
        }
        if(n == 5){
            vertice = atoi(p);
        }
        if(n == 6){
            distancia = atoi(p);
        }
        p = strtok (NULL, ";");
        n++;
      }
    strcpy(lugares[i - 1].nome,nome_aresta);
    lugares[i - 1].vertice_prox = vertice;
    lugares[i - 1].dist_vertice = distancia;
    n=1;
}

void escolhe_vertice(tlugares *lugares, int qtd_linhas, int *v_inicial, int *v_final){
      int i, id_inicial, id_final;
      printf("\nID:\tNOME:\t\n");
      for(i = 0; i < qtd_linhas; i++){
              printf("%d\t%s\n", (i+1), lugares[i].nome);
      }
      do{
        printf("\nEscolha o ID da regiao atual: \n");
        scanf("%d", &id_inicial);
        printf("\nEscolha o ID da regiao desejada: \n");
        scanf("%d", &id_final);
        if((id_inicial <= 0 || id_inicial > 74) || (id_final <= 0 || id_final > 74) || (id_final == id_inicial)){
            printf("\nTente novamente...\n");
        }
      }while((id_inicial <= 0 || id_inicial > 74) || (id_final <= 0 || id_final > 74) || (id_final == id_inicial));
      *v_inicial = lugares[id_inicial - 1].vertice_prox;
      *v_final = lugares[id_final - 1].vertice_prox;
}

void menorCaminho(tlugares *lugares, tgrafo *grafo, int s, int t, int qtd_vert, int qtd_linhas){ //procurar outro algoritmo se não der boa
    int dist[qtd_vert], perm[qtd_vert], path[qtd_vert];                                           
    int current, i, k, dc;  
    int smalldist, newdist;
    
    for (i = 0; i < qtd_vert; i++) {
        perm[i] = NAO_MEMBRO;
        dist[i] = INFINITO;
    }
    perm[s] = MEMBRO;
    dist[s] = 0;
    current = s;
    k = current;

    while (current != t) {
        smalldist = INFINITO;
        dc = dist[current];

        for (i = 0; i < qtd_vert; i++) {
            if (perm[i] == NAO_MEMBRO) {
                newdist = dc + grafo->arcos[current][i].tamanho;
                if (newdist < dist[i]) {
                    dist[i] = newdist;
                    path[i] = current;
                }
                if (dist[i] < smalldist) {
                    smalldist = dist[i];
                    k = i;
                }

            }
        }
        if (current == k) {
            printf("\n\nCAMINHO NAO EXISTE\n\n");
            return;
        }
        current = k;
        perm[current] = MEMBRO;
    }
    printf("\n\nRESULTADO: ");
    int caminho = t;

    printf("%d <- ", t);

    while (caminho != s)
    {
        printf("%d", path[caminho]);
        caminho = path[caminho];

        if (caminho != s)
            printf (" <- ");
    }

    printf("\n\ncusto: %d\n\n", dist[t]);

}
