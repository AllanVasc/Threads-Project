/*

Projeto de Threads 2020.3
Data: 21/10/20
Grupo:  Allan Soares Vasconcelos <asv>
        Amanda Lima Lassere <all2>
        Macio Monteiro de Meneses Jr <mmmj>
        Maria Isabel Fernandes dos Santos <mifs>

1. Você deverá criar um programa usando pthreads, 
no qual n threads deverão incrementar um contador global  
até o número 1.000.000.  
A thread que alcançar este valor deverá imprimir que o valor 
foi alcançado e todas as threads deverão finalizar a execução.

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

long contador = 0;      //Contador Global
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;  //Mutex que sera feito para impedir a condição de corrida!

void* inc(void *threadid){  //Função que será realizada pelas Threads 

  int tid = *((int *)threadid);   
  printf("Thread #%d inicializando...\n", tid); 

  while(contador < 1000000){

    pthread_mutex_lock(&myMutex);
    contador++;
    //printf("Thread: [%d] colocou contador em: [%ld]\n", tid, contador); //Somente para Debugar!

    if(contador == 1000000){

      printf("A Thread: #%d atingiu o numero 1000000!\n", tid); 

    }

    pthread_mutex_unlock(&myMutex);
  }
  pthread_exit(NULL);
}

int main (int argc, char *argv[]){   

  pthread_t *threads;
  int *taskids;
  int qtdThreads,threadCreator,i;

  scanf("%d", &qtdThreads);
  threads = (pthread_t*) malloc(qtdThreads*sizeof(pthread_t));
  taskids = (int *) malloc(qtdThreads*sizeof(int));

  for(i = 0; i < qtdThreads; i++){  

    taskids[i] = i;
	  printf("Main: criando thread #%d\n", i);   //Somente Debugando!   
    threadCreator = pthread_create(&threads[i], NULL, inc, (void *) &taskids[i]);     

    if (threadCreator){  //Garantir que a Thread foi criada corretamente!

      printf("ERRO; código de retorno é %d\n", threadCreator);         
      exit(-1);  

    }   
  }   
  pthread_exit(NULL);
}