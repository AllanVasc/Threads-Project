/*

Projeto de Threads 2020.3
Data: 31/10/20
Grupo:  Allan Soares Vasconcelos <asv>
        Amanda Lima Lasserre <all2>
        Macio Monteiro de Meneses Jr <mmmj>
        Maria Isabel Fernandes dos Santos <mifs>

5. (...) Nesta questão, o  objetivo  é quebrar a execução seqüencial em threads, 
na qual o valor de cada incógnita xi  pode ser calculado de forma concorrente 
em relação às demais incógnitas (Ex: x1(k+1) pode ser calculada ao mesmo tempo que x2(k+1)). 
A quantidade de threads a serem criadas vai depender de um parâmetro N passado pelo usuário 
durante a execução do programa, e N deverá ser equivalente à quantidade de processadores (ou núcleos) 
que a máquina possuir. No início do programa, as N threads deverão ser criadas, I incógnitas igualmente 
associadas para thread, e nenhuma thread poderá ser instanciada durante a execução do algoritmo. 
Dependendo do número N de threads, alguma thread poderá ficar com menos incógnitas assoicadas à ela.

(Continuação no PDF)

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define QTD_VAR 2       //Modificar esse valor para a quantidade de variaveis do SEL
#define QTD_INTER 10    //Modificar esse valor para definir a quantidade de interações

double matrizAInput[QTD_VAR][QTD_VAR] = { 2, 1, 
                                          5, 7 };   //Variaveis estaticas para a entrada! Modificar aqui!
double matrizBInput[] = {11, 13};                       //Variaveis estaticas para a entrada! Modificar aqui!
int qtdThreads = 0;
double lastInterX[QTD_VAR]; //Vai guardar a penultima interação do calculo do X
double newInterX[QTD_VAR];  //Vai guardar a ultima interação do calculo do x
pthread_barrier_t myBarrier;

/* 

Para a realização dessa questão, cada Thread possuira um ID, esse ID sera responsavel por informar qual
X(i) ela sera responsavel por calcular, sendo cada thread responsavel pelo X(ID % qtdThreads).

*/

void* jacobi(void *threadid){  //Função que será realizada pelas Threads 

    int tid = *((int *)threadid);   
    int i, j, k;
    double sum = 0;

    for(k = 0; k < QTD_INTER; k++){ //Realização do calculo do Xi!

        for(i = tid; i < QTD_VAR; i += qtdThreads){

            sum = 0;
            
            for(j = 0; j < QTD_VAR; j++){   //Realizando somatorio mais interno!

                if(i != j){

                    sum += matrizAInput[i][j] * lastInterX[j];

                }

            }

            newInterX[i] = (matrizBInput[i] - sum)/matrizAInput[i][i];

        }

        pthread_barrier_wait(&myBarrier);   //Nesse ponto, precisamos que todos os Xi(k+1) sejam calculados para prosseguir ao Xi(k+2)
        
        for(i = tid; i < QTD_VAR; i += qtdThreads){ //Somente debugando!
                    
            printf("Interacao: [#%d]: X[%d] = %lf\n", k, i+1, newInterX[i]);
            lastInterX[i] = newInterX[i];
        }

        pthread_barrier_wait(&myBarrier);
    }

    pthread_exit(NULL);

}

int main (int argc, char *argv[]){ 

    pthread_t *threads;
    int *taskids;
    int threadCreator, i;

    for(i = 0; i < QTD_VAR; i++){   //Questão pede para os valores serem inicializados com 1!

        lastInterX[i] = 1;

    }
    
    printf("Digite a quantidade de Threads a serem criadas...\n");
    scanf("%d", &qtdThreads);

    threads = (pthread_t*) malloc(qtdThreads*sizeof(pthread_t));    //Criando vetor que tera as threads
    taskids = (int *) malloc(qtdThreads*sizeof(int));
    pthread_barrier_init(&myBarrier, NULL, qtdThreads);     //Inicializando a barreira!

    for(i = 0; i < qtdThreads; i++){    //Realizar a criação das threads!  
        
        taskids[i] = i;
	    printf("Main: criando thread #%d\n", i);   //Somente Debugando!   
        threadCreator = pthread_create(&threads[i], NULL, jacobi, (void *) &taskids[i]);     

        if (threadCreator){  //Garantir que a Thread foi criada corretamente!

            printf("ERRO; código de retorno é %d\n", threadCreator);         
            exit(-1);  

        }   
    }

    for(i = 0; i < qtdThreads; i++){    //Esperando a finalização de todas as threads!

        pthread_join(threads[i], NULL);

    }

    printf("Todas as interacoes foram concluidas!\n");
    printf("Metodo Jacobi ---- RESULTADOS:\n");

    for(i = 0; i < QTD_VAR; i++){

        printf("X[%d] = %lf\n", i+1, newInterX[i]);

    }


    pthread_barrier_destroy(&myBarrier);

    pthread_exit(NULL);
}

// gcc -pthread questao05.c -o questao05 && ./questao05 //COMPILAR e EXECUTAR!