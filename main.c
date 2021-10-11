/*
PROG2-2020_2020100905_2020101428.c
Criado em: 03/12/2020
Autores: Arthur Purcino e Yuri Aikau
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define TAM 35
#define CASOS 202362

//Struct para armazenar datas
typedef struct{
  int ano;
  int mes;
  int dia;
}tData;

//Struct para armazenar as cidades e dados auxiliares
typedef struct {
  int id;
  char nome[TAM];
  int aux_casosTotais;
  int aux_casosData;
} tCidade;

//Struct para armazenar os dados que foram providenciados
typedef struct{
  int id;
  tData dataCadastro;
  tData dataObito;
  char classificacao[TAM];
  tCidade cidade;
  tData idadeNaDataNotificacao;
  char comorbidadePulmao[TAM]; 
  char comorbidadeCardio[TAM];
  char comorbidadeRenal[TAM];
  char comorbidadeDiabetes[TAM];
  char comorbidadeTabagismo[TAM];
  char comorbidadeObesidade[TAM];
  char ficouInternado[TAM];
  
} tInfo;

//Vetor principal que servira para armazenar todas as informacoes de casos de covid19 no ES
tInfo dados[CASOS-1];

//Armazenamento do numero de cidades lidas no arquivo covid19ES.csv
int totalCidades = 0;

//Vetor para armazenamento das informacoes das cidades
tCidade cidades[77];

//Funcao para a criacao da pasta dos arquivos de saida 
void folderCreator(char *pasta);

//Responsavel por ler cada linha do covid19ES.csv e armazenar os casos em um vetor do tipo tInfo
void leInfo(FILE *temp);

//Funcao para a impressao de cidades que possuem mais de n casos de covid19 no ES
void item3(int n, char *dir);

//Funcao que demonstra a quantidade de pessoas infectadas entre duas (d1, d2) datas sendo uma delas o limite inferior e a outra superior desse intervalo
void item4(char *d1, char *d2, char *dir);

//Funcao para a impressao de cidades que tiveram mais de n casos de covid no ES entre as datas data1 e data2
void item5(int n, char *data1, char *data2, char *dir);

void item6(char *nomeMunicipio, char*dir);

//Funcao inteiramente responsavel por lidar com o item 7, onde nela é feito os calculos de media, variacao/desvio padrao e a porcentagem de pessoas que morreram sem comorbidade em um determinado periodo de tempo (limitado inferiormente por d1 e superiormente por d2)
void item7 (char *d1, char *d2, char *dir);

//Funcao para verificacao de qual data eh mais antiga. Caso a primeira entrada seja mais antiga retorna 1, caso contrario 0
int dataMaisAntiga(tData antiga, tData nova);

void initializeCidade(tInfo dado, char *nome);

//Funcao responsavel por ler as entradas do programa
void leInputs();

//Funcao responsavel por converter uma string em inteiros respectivos a ano, mes e dia
tData filtroDatas(char *data);

//Funcao que le a string correspondente a Classificacao e retorna '1' se for confirmado e '0' as demais opcoes
int casoConfirmado(tInfo dado);

//Funcao que le a string correspondente a Ficouinternado e retorna '1' se a pessoa estava internada e '0' as demais opcoes
int ficouInternado(tInfo dado);

//Funcao que le a informacao armazenada dataObito e verifica se os valores sao iguais a '0000-00-00' para retornar '0' e '1' se for diferente
int morreu(tInfo dado);

//Funcao que le todas as strings relacionadas a comorbidade e se alguma delas for igual a "Sim" retorna '1', se nao, retorna '0'
int temComorbidade(int i);



int main(void) {
  //Abertura do arquivo covid19ES.csv
  FILE *temp;
  temp = fopen ("covid19ES.csv", "r");
  if (temp == NULL) {
    printf("Erro ao abrir o arquivo covid19ES.csv");
    return 1;
  }
  
  //Armazenamento de todos casos presentes no arquivo covid19ES.csv
  leInfo(temp);
  leInputs();

  //Fechamento do arquivo covid19ES.csv
  fclose(temp);
  return 0;
}

void leInfo(FILE *temp){
  int i;
  char str[200], aux_data[25], aux_obito[15], aux_idade[20], aux_final[40], nomeCidade[25];
  for (i = -1; i < CASOS; i++){
    //Uso do fscanf para leitura de cada linha do covid19ES.csv
    fscanf (temp, "%[^\n]\n", str);
    if (i >= 0) {
      dados[i].id = i;
      
      //Uso do sscanf para ler cada informacao de cada linha arquivo acima
      sscanf(str, "%[^,],%[^,],%[^,],%[^,],\"%[^\"]\",%[^2]", aux_data, aux_obito, dados[i].classificacao, nomeCidade, aux_idade, aux_final);
      //Tambem, foram utilizadas varias strings auxiliares que servirao para separar melhor as informacoes e armazena-las posteriormente

      dados[i].dataCadastro = filtroDatas(aux_data);
      dados[i].dataObito = filtroDatas(aux_obito);

      //Funcao para armazenar todos os nomes de cidade sem repeti-las
      initializeCidade(dados[i], nomeCidade);
      
      //Ja aqui eh usado o sscanf para separar a aux_idade em inteiros referentes a ano, mes e dia
      sscanf(aux_idade, "%d%*[^,], %d%*[^,], %d%*[^,]", &dados[i].idadeNaDataNotificacao.ano, &dados[i].idadeNaDataNotificacao.mes, &dados[i].idadeNaDataNotificacao.dia);

      //Nessa eh usado o sscanf para separar todas as demais informacoes e armazena-las em seus determinados campos
      sscanf(aux_final, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^2]", dados[i].comorbidadePulmao, dados[i].comorbidadeCardio, dados[i].comorbidadeRenal, dados[i].comorbidadeDiabetes, dados[i].comorbidadeTabagismo, dados[i].comorbidadeObesidade,dados[i].ficouInternado);

    }
  }
}

void leInputs(){
  char diretorio[20], cidade[40], aux_d1[15], aux_d2[15];
  int n1, n2;

  scanf("./%s/", diretorio);
  folderCreator(diretorio);
  
  scanf("%d", &n1);
  item3(n1, diretorio);
  
  scanf("%s %s", aux_d1, aux_d2);
  item4(aux_d1, aux_d2, diretorio);
  
  scanf("%d %s %s", &n2, aux_d1, aux_d2);
  item5(n2, aux_d1, aux_d2, diretorio);
  
  scanf("%s", cidade);
  item6(cidade, diretorio);
  
  scanf("%s %s", aux_d1, aux_d2);
  item7(aux_d1, aux_d2, diretorio);
}

void initializeCidade(tInfo dado, char *nome){
  int verify = 0;
  tCidade c;

  //Verificacao dentro do vetor cidades se essa ja esta inserida
  for (int i = 0; i <= totalCidades; i++) {
    if (!strcmp(cidades[i].nome, nome)) {
      verify = 1;
      c = cidades[i];
    }
  }
  
  if (verify) {
    dado.cidade = c;
    dados[dado.id] = dado;
  } else {
    //Caso nao esteja inserida, criacao de um novo tCidade e insercao dentro do vetor cidades
    strcpy(c.nome, nome);
    c.id = totalCidades;
    c.aux_casosData = 0;
    c.aux_casosTotais = 0;
    dado.cidade = c;
    dados[dado.id] = dado;
    cidades[totalCidades] = c;
    totalCidades += 1;
  }
}

tData filtroDatas (char *l){
  tData data;
  sscanf (l, "%d-%d-%d", &data.ano, &data.mes, &data.dia);
  return data;
}

void item3(int n, char *dir){
  int i, j;

  //Abertura do arquivo item3.txt para o output
  char item[13] = "/item3.txt", temp[11];
  FILE *item3;

  strcpy(temp, dir);
  strcat(temp,item);
  item3 = fopen (temp, "w");
  
  //Analise de cada caso confirmado e soma ao auxiliar de calculo de casos totais de cada cidade
  for (i = 0; i < CASOS; i++) {
    if (casoConfirmado(dados[i])) {
      dados[i].cidade.aux_casosTotais += 1;
      cidades[dados[i].cidade.id].aux_casosTotais += 1;
    }
  }
  
  //Copiando o vetor cidades para ordenadas
  tCidade ordenadas[77];
  for (i = 0; i < totalCidades; i++) {
    
    ordenadas[i] = cidades[i];
  }

  //Verificacao de cada nome de cidade, se sua inicial for maior que a proxima do vetor ordenadas, elas trocam de posicao.
  tCidade aux;
  for(j = 0; j < totalCidades; j++) {
    for(i = 0; i < totalCidades-1; i++){
      if(strcmp(ordenadas[i].nome, ordenadas[i + 1].nome) > 0) {
        aux = ordenadas[i];
        ordenadas[i] = ordenadas[i+1];
        ordenadas[i+1] = aux;
      }
    }
  }

  //Output no arquivo item3.txt do resultado
  for (i = 0; i < totalCidades; i++) {
    if (ordenadas[i].aux_casosTotais > n){
      fprintf(item3 ,"- %s: %d casos\n", ordenadas[i].nome, ordenadas[i].aux_casosTotais);
    }
  }
}

void item4(char *data1, char *data2, char *dir) {
  tData d1 = filtroDatas(data1), d2 = filtroDatas(data2);

  //Abertura do arquivo item4.txt para o output
  int total = 0;
  char item[13] = "/item4.txt", temp[11];
  FILE *item4;

  strcpy(temp, dir);
  strcat(temp,item);

  item4 = fopen(temp, "w");

  //Verificacao de se o caso esta entre as datas e a soma ao total de casos confirmados
  for (int i = 0; i < CASOS; i++) {
    tInfo inf = dados[i];
    if (dataMaisAntiga(d1, dados[i].dataCadastro) && dataMaisAntiga(dados[i].dataCadastro, d2) && casoConfirmado(dados[i])) {
      total += 1;
    }
  }
  fprintf(item4, "- Total de pessoas: %d\n", total);
}

void item5(int n, char* data1, char* data2, char *dir){
  tData d1 = filtroDatas(data1), d2 = filtroDatas(data2);
  //Abertura do arquivo item5.txt para o output
  int i, j;
  char item[13] = "/item5.txt", temp[11];
  FILE *item5;

  strcpy(temp, dir);
  strcat(temp,item);

  item5 = fopen(temp, "w");

  //Verificacao de se o caso esta entre as datas e a soma aos casos confirmados da cidade
  for ( i = 0; i < CASOS; i++) {
    if (dataMaisAntiga(d1, dados[i].dataCadastro) && dataMaisAntiga(dados[i].dataCadastro, d2) && casoConfirmado(dados[i])) {
      dados[i].cidade.aux_casosData += 1;
      cidades[dados[i].cidade.id].aux_casosData += 1;
    }
  }
  
  
  //Copiando o vetor cidades para ordenadas
  tCidade ordenadas[77];
  for (i = 0; i < totalCidades; i++) {
    ordenadas[i] = cidades[i];
  }


  //Verificacao do numero de casos de cada cidade, se obtiver menos casos que a cidade seguinte no vetor ordenadas, trocam de posicao dentro do vetor.
  tCidade aux;
  for(j = 0; j < totalCidades; j++) {
    for(i = 0; i < totalCidades-1; i++){
      if(ordenadas[i+1].aux_casosData > ordenadas[i].aux_casosData) {
        aux = ordenadas[i];
        ordenadas[i] = ordenadas[i+1];
        ordenadas[i+1] = aux;
      }
    }
  }

//Output desejado no item5.txt
  for (i = 0; i < n; i++) {
    if (ordenadas[i].aux_casosData > n){
      fprintf(item5 ,"%s: %d casos\n", ordenadas[i].nome, ordenadas[i].aux_casosData);
    }
  }
}

void item6(char *nomeMunicipio, char*dir){
  //Abertura do arquivo item6.txt para o output
  int i;
  char item[13] = "/item6.txt", temp[11];
  FILE *item6;

  strcpy(temp, dir);
  strcat(temp,item);

  item6 = fopen(temp, "w");

  float internadasConfirmadas = 0, morreuConfirmadas = 0, confirmados = 0, internadas = 0, morreram = 0, internadasNconfirmados = 0;
  
  //Colocando em caixa alta todas as letras do input
  char nomeFormatado[strlen(nomeMunicipio)];
  for (i = 0; i <= strlen(nomeMunicipio); i++)  {
    nomeFormatado[i] = toupper(nomeMunicipio[i]);
  }

  //Analise de cada caso e adicao a seu int
  if (strcmp("TODAS", nomeFormatado) == 0) {
    for (i = 0; i < CASOS; i++) {
      if (casoConfirmado(dados[i])) {
        confirmados += 1;
        if (ficouInternado(dados[i])) {
          internadasConfirmadas += 1;
        }
        if (morreu(dados[i])) {
          morreuConfirmadas += 1;
        }
        if (ficouInternado(dados[i]) && (morreu(dados[i]))) {
          morreram++; 
        }
      }
    }
  }else {
    for (i = 0; i < CASOS; i++) {
      if (strcmp(dados[i].cidade.nome, nomeFormatado) == 0) {
        if (casoConfirmado(dados[i])) {
          confirmados += 1;
          if (ficouInternado(dados[i])) {
            internadasConfirmadas += 1;
          }
          if (morreu(dados[i])) {
            morreuConfirmadas += 1;
          }
          if (ficouInternado(dados[i]) && (morreu(dados[i]))) {
            morreram++; 
          }
        }
      } 
    }
  }

  
  internadas = internadasConfirmadas + internadasNconfirmados;

  fprintf(item6, "- Resultado para %s:\n", nomeFormatado);
  fprintf(item6, "- A %% de pessoas com Covid-19 que ficaram internadas: %.3f%%\n", 100*(internadasConfirmadas/confirmados));
  fprintf(item6, "- A %% de pessoas com Covid-19 que morreram: %.3f%%\n", 100*(morreuConfirmadas/confirmados));
  fprintf(item6, "- A %% de pessoas que ficaram internadas e morreram: %.3f%%\n", 100*(morreram/morreuConfirmadas));

}

void item7(char* data1, char* data2, char *dir){
  tData d1 = filtroDatas(data1), d2 = filtroDatas(data2);
  
  int j;
  float tot = 0, med = 0, var = 0, sem_comorbidade = 0, idade_float = 0, DP, v;
  tData dataNova;
  char item[13] = "/item7.txt", temp[11];
  FILE *item7;

  strcpy(temp, dir);
  strcat(temp,item);

  item7 = fopen(temp, "w");

  for (j = 0; j < CASOS; j++){
    dataNova = dados[j].dataCadastro;
    //If usado para verificar se a data na posicao 'j' esta entre a d1 e d2 fornecidas
    if (dataMaisAntiga(d1, dataNova) && (dataMaisAntiga(dataNova, d2))){
      //Verificacao se a pessoa esta morta e tinha um caso confirmado
      if (morreu(dados[j]) && casoConfirmado(dados[j])){
        //Quantidade de pessoas que morreram nesse periodo de tempo e que estavam com caso comfirmado
        tot++;
        idade_float = dados[j].idadeNaDataNotificacao.ano;
        med += idade_float;
        //Checa se essa pessoa tinha alguma comorbidade
        if (!temComorbidade(j)){
          sem_comorbidade++;
        }
      }
    }
  }
  //Calculo da media
  med = med/tot;
  //Loop para fazer o calculo da variacao
  for (j = 0; j < CASOS; j++){
    dataNova = dados[j].dataCadastro;
    if ((dataMaisAntiga(d1, dataNova)) && (dataMaisAntiga(dataNova, d2)) && (casoConfirmado(dados[j]))){
      if (morreu(dados[j])){
        idade_float = (float)dados[j].idadeNaDataNotificacao.ano;
        v = ((idade_float) - (med));
        var += (pow(v, 2));
      }
    }
  }
  //Calculo de desvio padrao
  DP = sqrt(var/(tot-1));

  fprintf(item7, "A media e desvio padrao da idade: %.3f -- %.3f\n", med, DP);
  fprintf(item7, "A %% de pessoas que morreram sem comorbidade: %.3f%%\n", (sem_comorbidade/tot)*100);
}

int dataMaisAntiga(tData antiga, tData nova) {
  if (antiga.ano < nova.ano) {
    return 1;
  } else if (antiga.ano == nova.ano) {
    if (antiga.mes < nova.mes) {
      return 1;
    } else if (antiga.mes == nova.mes) {
      if (antiga.dia <= nova.dia) {
        return 1;
      } else {
        return 0;
      }
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void folderCreator(char *pasta) {
  //Uso do terminal para a execucao do comando mkdir, usado para criar a pasta
  char buffer[100];
  sprintf(buffer, "mkdir %s", pasta);
  system(buffer);
}

int temComorbidade(int i){
  char sim[4] = "Sim";
  //Abaixo se faz uma comparacao de strings onde se apenas uma delas for verdadeira, toda a expressao se torna verdadeira e entra no 'if'
  if ((strcmp(dados[i].comorbidadePulmao, sim) == 0) || (strcmp(dados[i].comorbidadeCardio, sim) == 0) || (strcmp(dados[i].comorbidadeRenal, sim) == 0) || (strcmp(dados[i].comorbidadeDiabetes, sim) == 0) || (strcmp(dados[i].comorbidadeTabagismo, sim) == 0) || (strcmp(dados[i].comorbidadeObesidade, sim) == 0)){
    return 1;
  }else
  {
    return 0;
  }
  return 0;
}

int casoConfirmado(tInfo dado) {
  if (!strcmp(dado.classificacao, "Confirmados")) {
    return 1;
  } 
  return 0;
}

int ficouInternado(tInfo dado) {
  if (!strcmp(dado.ficouInternado, "Sim")) {
    return 1;
  }
  return 0;
}

int morreu(tInfo dado) {
  if (dado.dataObito.dia != 0) {
    return 1;
  }
  return 0;
}