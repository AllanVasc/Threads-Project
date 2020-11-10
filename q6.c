/*
Projeto de Threads 2020.3
Data: 02/11/20
Grupo:  Allan Soares Vasconcelos <asv>
        Amanda Lima Lassere <all2>
        Macio Monteiro de Meneses Jr <mmmj>
        Maria Isabel Fernandes dos Santos <mifs>

6. Em Java existem implementações de coleções (ex.: LinkedList, Set) que não apenas 
são seguras para o uso concorrente, mas são especialmente projetadas para suportar tal uso. 
Uma fila bloqueante (BlockingQueue) é uma fila limitada de estrutura FIFO (First-in-first-out)
que bloqueia uma thread ao tentar adicionar um elemento em uma fila cheia ou retirar de uma fila vazia.
Utilizando as estruturas de dados definidas abaixo e a biblioteca PThreads, crie um programa em C
do tipo produtor/consumidor implementando uma fila bloqueante de inteiros (int) com procedimentos
semelhantes aos da fila bloqueantes em Java.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int qtdConsumidores;
int qtdProdutores;
int cnt;

int produzido = 0;
int consumido = 0;

pthread_cond_t full, empty;
pthread_mutex_t qtdMutex, qtdMutexConsumidores, qtdMutexProdutores;

typedef struct elem{
   int value;
   struct elem *prox;
}Elem;
 
typedef struct blockingQueue{
   unsigned int sizeBuffer, statusBuffer;
   Elem *head,*last;
}BlockingQueue;


BlockingQueue*newBlockingQueue(unsigned int SizeBuffer){

        BlockingQueue* fila = malloc(sizeof(BlockingQueue));

        fila->sizeBuffer = SizeBuffer;
        fila->statusBuffer = 0;
        fila->head = NULL;
        fila->last = NULL;

        return fila;
}

void adicionarElem(BlockingQueue *fila, int v) { //add ao final da fila

    Elem *aux = (Elem *) malloc(sizeof(Elem));
    aux->value = v;
    aux->prox = NULL;

    if(fila->statusBuffer == 0){
        fila->head = aux;
        fila->last = aux;
    } else {
        
        fila->last->prox = aux;
        fila->last = aux;
    }

    fila->statusBuffer++;
}


int retirarElem(BlockingQueue *Q) {

    int result = Q->head->value;
    Elem *aux = Q->head;

    if(Q->head->prox != NULL && Q->statusBuffer == 2) {
        Q->head = Q->head->prox;
        Q->last = Q->head;
    } else if(Q->head->prox != NULL){
        Q->head = Q->head->prox;
    } else {
        Q->head = NULL;
        Q->last = NULL;
    }

    Q->statusBuffer--;

    free(aux);

    return result;
}

void putBlockingQueue(BlockingQueue* fila, int newValue){
    pthread_mutex_lock(&qtdMutex);

    // verifica se o buffer está cheio
    while(fila->sizeBuffer == fila->statusBuffer){
        printf("Fila cheia\n");
        // thread vai dormir
        pthread_cond_wait(&empty, &qtdMutex);
    }

    adicionarElem(fila, newValue);
    
    if(fila->statusBuffer == 1) {
        pthread_cond_broadcast(&full);
        // acorda as outras threads
    }

    pthread_mutex_unlock(&qtdMutex);    
}

int takeBlockingQueue(BlockingQueue* fila){
    pthread_mutex_lock(&qtdMutex);

    // verifica se o buffer está vazio
    while(fila->statusBuffer == 0){
        printf("Fila vazia\n");
        // thread vai dormir
        pthread_cond_wait(&full, &qtdMutex);
    }

    int result = retirarElem(fila);

    if(fila->statusBuffer == fila->sizeBuffer - 1){
        // acordar as outras threads
        pthread_cond_broadcast(&empty);
    }

    pthread_mutex_unlock(&qtdMutex); 

    return result;
}

void *produtor(BlockingQueue* Q){
    printf("Produtor \n");

    int i;
    for(i = 0; i < cnt * qtdConsumidores; i++){
        putBlockingQueue(Q, i);
        printf("Produzi: %d \n", i + 1);
    }

    pthread_exit(NULL);
}

void *consumidor(BlockingQueue* Q){
    printf("Consumidor \n");
    
    int i;

    for(i = 0; i < cnt * qtdProdutores; i++){
        
        int aux = takeBlockingQueue(Q);
        printf("Consumi: %d \n", aux + 1);
    }  

    pthread_exit(NULL);
}

void clear(BlockingQueue *Q){
    while(Q->statusBuffer > 0){
        int aux = retirarElem(Q);
    }

    free(Q);
}

int main() {

    unsigned int tamBuffer;

    printf("Digite o número de threads produtoras, threads consumidoras e o tamanho do buffer e o número de elementos no total: ");
    scanf("%d %d %u %d", &qtdProdutores, &qtdConsumidores, &tamBuffer, &cnt);

    BlockingQueue *fila = newBlockingQueue(tamBuffer);

    // array de threads
    pthread_t *consumer = (pthread_t *) malloc(qtdConsumidores * sizeof(pthread_t)); 
    pthread_t *producer = (pthread_t *) malloc(qtdProdutores * sizeof(pthread_t));

    int j;
    // Cria as threads produtoras
    for(j = 0; j < qtdProdutores; j++){
        pthread_create(&producer[j], NULL, (void *) produtor, (void *) fila);
    } 

    // Cria as threads consumidoras
    for(j = 0; j < qtdConsumidores; j++){
        pthread_create(&consumer[j], NULL, (void *) consumidor, (void *) fila);
    }

    // Esperar o término de execução das threads

    for(j = 0; j < qtdProdutores; j++){
        pthread_join(producer[j], NULL);
    } 

    for(j = 0; j < qtdConsumidores; j++){
        pthread_join(consumer[j], NULL);
    } 

    // Libera memória

    free(consumer);
    free(producer);
    clear(fila);

    pthread_exit(NULL);
}

// gcc -pthread questao06.c -o questao06 && ./questao06 //COMPILAR e EXECUTAR!