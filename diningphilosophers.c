#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
  
#define PENSAR 0        //Id para estado pensando
#define ESPERAR 1       //Id para estado esperando
#define COMER 2         //Id para estado comendo
 
typedef struct nfilosofos{      //Estrutura de dados dos filósofos
        int quant;              //Quantidade de filósofos
        int tempoP;             //Tempo para pensar
        int tempoC;             //Tempo para comer
        int id;                 //Índice do filósofo
}NFilosofos;
int *estado;            //Vetor que armazena os estados de cada thread
sem_t lugarMesa;        //Representa os lugares na mesa
sem_t *garfo;           //Vetor que representa os garfos
 
void *filosofo (void *F);
void pegarGarfo (NFilosofos F);
void deixarGarfo (NFilosofos F);
void mostrar (int i);
void teste (int i,int quant);
  
void main(){
         int quantFilo, tComer, tPensar, i;      //Variáveis de entrada
        NFilosofos *vFilo;       //Vetor que armazena as estruturas de dados dos filósofos
        pthread_t *thFilo;      //Vetor de Threads que exerceram a função dos filósofos
         do{    
                printf ("Digite a quantidade de Filosofos: ");
                scanf ("%d",&quantFilo);
                if (quantFilo<1){
                        printf("\nERROR\n\n");
                }
        }while (quantFilo<1);
         do{    
                printf ("Digite o tempo para comer (em segundos): ");
                scanf ("%d",&tComer);
                if (tComer<1){
                        printf("\nERROR\n\n");
                }
        }while (tComer<1);
 
        do{    
                printf ("Digite o tempo para pensar (em segundos): ");
                scanf ("%d",&tPensar);
                if (tPensar<1){
                        printf("\nERROR\n\n");
                }
        }while (tPensar<1);
 
        vFilo = (NFilosofos*) malloc ((quantFilo)*sizeof(NFilosofos));  //Aloca vetor de filósofos
        thFilo = (pthread_t*) malloc ((quantFilo)*sizeof(pthread_t));   //Aloca vetor de threads
        garfo = (sem_t*)malloc((quantFilo)*sizeof(sem_t));    //Aloca a quantidade de garfos = quantidade de filósofos
        estado = (int*)malloc((quantFilo)*sizeof(int));     //Aloca o vetor de estado dos filósofos
 
        sem_init(&lugarMesa,0,quantFilo-1); //Inicializa os semáforos dos lugares da mesa
        for (i=0;i<quantFilo;i++){           //Inicializa os semáforos de garfos
                sem_init(&garfo[i],0,1);}
 
        for (i=0;i<quantFilo;i++){         //Inicializa o vetor com os dados dos filósofos
                vFilo[i].quant = quantFilo;
                if ((i % 2) == 0){                 //Random no tempo de comer e de pensar
                        vFilo[i].tempoC = tComer + (rand() % (tComer/2));
                        vFilo[i].tempoP = tPensar - (rand() % (tPensar/2));
                }else{
                        vFilo[i].tempoP = tPensar + (rand() % (tPensar/2));
                        vFilo[i].tempoC = tComer - (rand() % (tComer/2));
                }
                vFilo[i].id = i + 1;
                estado[i] = PENSAR;         //Inicializa cada filósofo com o estado PENSAR
                pthread_create (&thFilo[i],NULL,filosofo,&vFilo[i]);   //Cria os filósofos
        }
 
        for (i=0;i<quantFilo;i++){            //Faz um join nos filósofos
                pthread_join (thFilo[i],NULL);
        }
 
        sem_destroy(&lugarMesa);
        for (i=0;i<quantFilo;i++){
                sem_destroy(&garfo[i]);
        }
 
        free(garfo);
        free(vFilo);
        free(thFilo);
        free(estado);
        pthread_exit(NULL);
       
}
 
//Função que simula o filósofo
void *filosofo (void *F){
        NFilosofos Filo = *(NFilosofos*) F;
       
        while (1){
                mostrar(Filo.id);
                usleep(Filo.tempoP * 1000);
                pegarGarfo(Filo);
                usleep(Filo.tempoC  * 1000);
                deixarGarfo(Filo);
        }
}
 

//Função que filósofo pega os garfos da mesa
void pegarGarfo (NFilosofos F){
        sem_wait(&lugarMesa);
        estado[F.id] = ESPERAR;
        mostrar(F.id);
        teste(F.id,F.quant);
        mostrar(F.id);
        sem_post(&lugarMesa);
        sem_wait(&garfo[F.id]);
}
 
//Função que filósofo deixa os garfos na mesa
void deixarGarfo (NFilosofos F){
        sem_wait(&lugarMesa);
        estado[F.id] = PENSAR;
        mostrar(F.id);
        teste((F.id + (F.quant - 1)) % F.quant,F.quant);//(i + quant - 1) % quant == ESQ
        teste((F.id + 1) % F.quant,F.quant);   //(i + 1) % quant == DIREITA
        sem_post(&lugarMesa);
}
 
//Função que mostra o estado do filósofo i
void mostrar (int i){
        if(estado[i] == PENSAR){
                printf("O filosofo %d esta pensando.\n", i);
        }
        if(estado[i] == ESPERAR){
                printf("O filosofo %d esta esperando.\n", i);
        }
        if(estado[i] == COMER){
                printf("O filosofo %d esta comendo.\n", i);
        }
}



 
//Função que testa se o filósofo pode comer
void teste (int i, int quant){
        if (estado[i] == ESPERAR && estado[(i + quant - 1) % quant] != COMER && estado[(i + 1) % quant] != COMER) { //Verifica se os filósofos da esquerda e direita não estão comendo para o filósofo i poder comer
                estado[i] = COMER;
                sem_post(&garfo[i]);
        }
}



