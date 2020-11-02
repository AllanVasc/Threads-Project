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

#define qtdConsumidores 15
#define qtdProdutores 1
#define cnt 30

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

        // logica da funcao

        return fila;
}
void putBlockingQueue(BlockingQueue*Q,intnewValue){

        // logica da funcao

}
int takeBlockingQueue(BlockingQueue* Q){



}

int main(int argc, char *argv[]) {


        pthread_exit(NULL);
}