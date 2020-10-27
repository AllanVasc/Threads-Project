/*

Projeto de Threads 2020.3
Data: 27/10/20
Grupo:  Allan Soares Vasconcelos <asv>
        Amanda Lima Lassere <all2>
        Macio Monteiro de Meneses Jr <mmmj>
        Maria Isabel Fernandes dos Santos <mifs>

7. Você deverá implementar um programa  que converte imagens colorida em tons de cinza 
utilizando pthreads (para acelerar a conversão). As imagens deverão adotar o modelo de 
cores RGB: Red, Green e Blue. Este é um modelo aditivo, no qual as cores primárias 
vermelho, verde e azul são combinadas para produzir uma cor.  Desta forma, em uma imagem 
do tipo bitmap (matricial), cada pixel possui 3 valores. 
O formato textual PPM (Portable Pixel Map) do tipo P3 será adotado e, abaixo, segue um exemplo:

(Continuação no PDF)

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Pixel {

    int red;
    int green;
    int blue;

};

struct Pixel **bitMapInput;
struct Pixel **bitMapOutput;    //Variavel global onde sera acessada por meio das Threads!
int qtdLinhas = 0, qtdColunas = 0;

void* grayConversion(void *threadid){  //Função que será realizada pelas Threads 

    int tid = *((int *)threadid);   
    int linhaThread, colunaThread;

    linhaThread = tid / qtdColunas;
    colunaThread = tid % qtdColunas;

    int gray =  (bitMapInput[linhaThread][colunaThread].red)*0.30 + 
                (bitMapInput[linhaThread][colunaThread].green)*0.59 +  //Realizando calculo do novo valor!
                (bitMapInput[linhaThread][colunaThread].blue)*0.11;

    printf("#%d ---> [%d][%d] = [%d]\n", tid, linhaThread, colunaThread, gray); 

    bitMapOutput[linhaThread][colunaThread].red = gray;
    bitMapOutput[linhaThread][colunaThread].green = gray;
    bitMapOutput[linhaThread][colunaThread].blue = gray;

    pthread_exit(NULL);

}

int main (int argc, char *argv[]){ 

    FILE * arquivoEntrada;
    pthread_t *threads;
    int *taskids;

    char formato[5];
    int valorMaximo = 0;
    int threadCreator, i,k, qtdThreads, pixel = 0, redInput, greenInput, blueInput;
    
    arquivoEntrada = fopen("questao07Input.txt", "r");  //Abrindo o arquivo

    if(arquivoEntrada == NULL){
        printf("ERRO AO ABRIR ARQUIVO!\n");
        exit(-1);
    }

    fscanf(arquivoEntrada, "%s", formato);              //Lendo a 1ª Linha!

    if (strcmp(formato, "P3") != 0){
        printf("FORMATO NAO SUPORTADO!\n");
        exit(-1);
    }

    //printf("Li: %s\n", formato);    //Debugando

    fscanf(arquivoEntrada, "%d %d\n", &qtdColunas, &qtdLinhas);   //Lendo a 2ª Linha!

    //printf("Li: %d e %d\n", qtdColunas, qtdLinhas);

    bitMapInput = (struct Pixel **) malloc(qtdLinhas*sizeof(struct Pixel *));     //Criando a matriz de input!

    for(i = 0; i < qtdLinhas; i++){

        bitMapInput[i] = (struct Pixel *) malloc(qtdColunas*sizeof(struct Pixel));

    }

    fscanf(arquivoEntrada, "%d\n", &valorMaximo);   //Lendo a 3ª Linha!

    //printf("Li: %d\n", valorMaximo);  //Debugando

    for(i = 0, pixel = 0; i < qtdLinhas; i++){ 

        for(k = 0; k < qtdColunas; k++){

            fscanf(arquivoEntrada, "%d %d %d\n", &redInput, &greenInput, &blueInput); //Lendo Matriz!

            bitMapInput[i][k].red = redInput;
            bitMapInput[i][k].green = greenInput;
            bitMapInput[i][k].blue = blueInput;

            if(redInput > valorMaximo || greenInput >> valorMaximo || blueInput > valorMaximo){

                printf("Valor maximo excedido!\n");
                exit(-1);
            }

            //printf("[%d]: %d %d %d\n", pixel, bitMapInput[i][k].red, bitMapInput[i][k].green, bitMapInput[i][k].blue); //Debugando!
            pixel++;

        }
    }

    // ---------------------------- Fim da Leitura dos dados ---------------------------------

    qtdThreads = qtdLinhas*qtdColunas;

    threads = (pthread_t*) malloc(qtdThreads*sizeof(pthread_t));    //Criando vetor que tera as threads
    taskids = (int *) malloc(qtdThreads*sizeof(int));

    bitMapOutput = (struct Pixel **) malloc(qtdLinhas*sizeof(struct Pixel *));     //Criando a matriz de OUTPUT!

    for(i = 0; i < qtdLinhas; i++){

        bitMapOutput[i] = (struct Pixel *) malloc(qtdColunas*sizeof(struct Pixel));

    }

    for(i = 0; i < qtdThreads; i++){    //Realizar a criação das threads!  
        
        taskids[i] = i;
	    printf("Main: criando thread #%d\n", i);   //Somente Debugando!   
        threadCreator = pthread_create(&threads[i], NULL, grayConversion, (void *) &taskids[i]);     

        if (threadCreator){  //Garantir que a Thread foi criada corretamente!

            printf("ERRO; código de retorno é %d\n", threadCreator);         
            exit(-1);  

        }   
    }

    for(i = 0; i < qtdThreads; i++) {   //Necessario esperar todas as threads finalizarem para colocar o resultado final!

        pthread_join(threads[i], NULL);

    }

    printf("Resultado final:\n");

    for(i = 0, pixel = 0; i < qtdLinhas; i++){ 

        for(k = 0; k < qtdColunas; k++){

            printf("[#%d] - %d %d %d\n", pixel, bitMapOutput[i][k].red, bitMapOutput[i][k].green, bitMapOutput[i][k].blue); //Debugando!
            pixel++;

        }
    }

    fclose(arquivoEntrada);

    // ---------------------------- Fim da Manipulação dos dados  ---------------------------------

    arquivoEntrada = fopen("questao07Output.txt", "wt");

    if(arquivoEntrada == NULL){
        printf("ERRO AO ABRIR ARQUIVO!\n");
        exit(-1);
    }

    for(i = 0, pixel = 0; i < qtdLinhas; i++){ 

        for(k = 0; k < qtdColunas; k++){

            fprintf(arquivoEntrada, "[#%d] - %d %d %d\n", pixel,
                                                        bitMapOutput[i][k].red, 
                                                        bitMapOutput[i][k].green, 
                                                        bitMapOutput[i][k].blue); //Salvando resultados no arquivo!
            pixel++;

        }
    }

    fclose(arquivoEntrada);

    // ---------------------------- Fim da Criação do Arquivo ---------------------------------

    printf("Programa Finalizado!\n");
    pthread_exit(NULL);

}

// gcc -pthread questao07.c -o questao07 && ./questao07 //COMPILAR e EXECUTAR!