/*

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

void *inc(void *threadid){

    int threadID = *((int *)threadID);
    contador++;
    if(contador >= 1000000){

        printf("A Thread: #%d atingiu o numero 1000000!\n", threadID); 
        pthread_exit(NULL);
    }
}

int main (int argc, char *argv[]){   

    pthread_t *threads;
    int *taskids;
    int qtdThreads = 0, i = 0, threadsAtual;

    scanf("%d", &qtdThreads);
    threads = (pthread_t*) malloc(qtdThreads*sizeof(pthread_t));
    taskids = (int *) malloc(qtdThreads*sizeof(int));

    for(i = 0; i < qtdThreads; i++){

	    printf("No main: criando thread %d\n", i);
        taskids[i] = i;
        threadsAtual = pthread_create(&(threads[i]), NULL, inc, (void *) taskids[i]);  

        if (threadsAtual){      

            printf("ERRO; código de retorno é %d\n", threadsAtual);         
            exit(-1);   

        }   
    }

  pthread_exit(NULL);
}

// gcc -pthread questao01.c -o questao01  COMPILANDO
// ./questao01 EXECUTANDO

/*

void *inc(void *threadid){
  int i = 0; 
  for(; i < 9000000; i++) { 
    contador++; 
   }
}
void *dec(void *threadid){   
  int i = 0;
  for(; i < 9000000; i++) { 
    contador--; 
   }
}
int main (int argc, char *argv[]){   
  pthread_t thread1, thread2;   
  pthread_create(&thread1, NULL, inc, NULL); 
  pthread_create(&thread2, NULL, dec, NULL); 
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL); 
  printf("Valor final do contador: %ld\n", contador);
  pthread_exit(NULL);
}

*/