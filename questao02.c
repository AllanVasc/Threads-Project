/*

Projeto de Threads 2020.3
Data: 02/11/20
Grupo:  Allan Soares Vasconcelos <asv>
        Amanda Lima Lassere <all2>
        Macio Monteiro de Meneses Jr <mmmj>
        Maria Isabel Fernandes dos Santos <mifs>

2. Em algumas estações de trem,  existem telas que informam os trens 
que estão chegando e partindo. Estas telas são atualizadas sempre que 
uma nova informação situação dos trens está disponível. Você deve 
implementar um programa usando threads que atualize esta tela de acordo 
com as informações lidas de arquivos. Uma tela com 7 linhas possui o 
seguinte formato:

(Continuação no PDF)

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define QTD_CHAR_LINHA 25   //Modificar aqui para permitir tabelas maiores! 
#define QTD_COLORS 8

char colors[QTD_COLORS][10] = {"\e[41m", "\e[43m", "\e[44m", "\e[45m", "\e[42m", "\e[40m", "\e[46m", "\e[47m"};  //Necessario para usar no formato ANSI
char reset[10] = "\e[0m";

// printf("%s" "%s" "%s\n", colors[i], placa[i], reset);
// placa[i] é o que queremos printar

pthread_mutex_t *myVectorMutex; //Vetor de Mutex que sera feito para impedir a condição de corrida!
pthread_mutex_t myMutexPrint = PTHREAD_MUTEX_INITIALIZER;  //Mutex que sera feito para controlar o print!
char **tabelaOutput;            //Variavel Global onde ira sofrer as modificações das threads!!
int qtdThreads, qtdArquivos, qtdLinhas;

/* 

Para realizar essa questão primeiro lemos a quantidade de arquivos, quantidade de threads e a quantidade 
de linhas que a tabela ira possuir, cada Thread ira ser responsavel pelo arquivo (ID % qtdThreads). Para
garantir a espera de 2s para cada linha ser modificada novamente criamos thread dentro da thread, prestar
atenção pois utilizamos uma variavel inteira para guardar os indices da threadWait, logo, cada thread
principal so pode realizar 2147483647 operações de mudança na tabela(Overflow ira acarretar em segmentation fault)

*/

void* waitThread(void *threadid){   //Função que será responsavel pelas ThreadsWait!
    
    int linhaWait = *((int *)threadid);
    sleep(2);
    pthread_mutex_unlock(&myVectorMutex[linhaWait]);
    pthread_exit(NULL);

}

void* changeTable(void *threadid){  //Função que será realizada pelas Threads 

    int tid = *((int *)threadid);
    FILE * arquivoEntrada;
    pthread_t *threadWait;
    char nomeArquivoInput[20];
    char nomeArquivoInputAux[20];
    char codigo[QTD_CHAR_LINHA + 1], localizacao[QTD_CHAR_LINHA + 1], horario[QTD_CHAR_LINHA + 1];
    char msgFormatada[QTD_CHAR_LINHA + 1];
    int i, j, linhaAtual, modifiquei, printei, qtdEspacos, threadWaitCreator;
    int qtdThreadsWait = 1; //Precisa tomar cuidado!Numero muito grande de operações ira fazer travar o programa!
    int *taskids;

    threadWait = (pthread_t*) malloc(qtdThreadsWait*sizeof(pthread_t));    //Criando vetor que tera as threadsWait
    taskids = (int *) malloc(qtdThreadsWait*sizeof(int));

    for(i = tid; i < qtdArquivos; i += qtdThreads){  //Cada Thread ficara responsavel pelo TID % qtdArquivo

        strcpy(nomeArquivoInput, "questao02Input");     //Criando a string do arquivo que sera lido!!
        sprintf(nomeArquivoInputAux, "%d", i);
        strcat(nomeArquivoInput, nomeArquivoInputAux);
        strcat(nomeArquivoInput, ".txt");

        arquivoEntrada = fopen(nomeArquivoInput, "r");  //Abrindo o arquivo

        if(arquivoEntrada == NULL){

            printf("ERRO AO ABRIR ARQUIVO [%s]\n", nomeArquivoInput);
            exit(-1);

        }

        while(!feof(arquivoEntrada)){     //Realizando a leitura de todo o arquivo

            modifiquei = 0;
            fscanf(arquivoEntrada, "%d ", &linhaAtual);
            fscanf(arquivoEntrada, "%s %s %s ", codigo, localizacao, horario);

            strcpy(msgFormatada, codigo); //Formatando a linha!
            strcat(msgFormatada, " ");
            strcat(msgFormatada, localizacao);

            qtdEspacos = QTD_CHAR_LINHA - (strlen(codigo) + 1 + strlen(localizacao) + strlen(horario));

            for(j = 0; j < qtdEspacos; j++){

                strcat(msgFormatada, " ");

            }

            strcat(msgFormatada, horario);

            while(modifiquei == 0){     //Vai realizar a mudança na variavel global!

                if ( pthread_mutex_trylock(&myVectorMutex[linhaAtual]) == 0){
                    
                    strcpy(tabelaOutput[linhaAtual], msgFormatada);
                    printei = 0;

                    while(printei == 0){    //Realizando o print da tabela!

                        if( pthread_mutex_trylock(&myMutexPrint) == 0){
                            
                            system("clear");
                            for(j = 0; j < qtdLinhas; j++){

                                printf("%s" "%s" "%s\n", colors[j % QTD_COLORS], tabelaOutput[j], reset);

                            }

                            pthread_mutex_unlock(&myMutexPrint);
                            printei = 1;
                        }
                   }

                    //Criando uma thread para ser responsavel por segurar a linha por 2s!
                    taskids[qtdThreadsWait - 1] = linhaAtual;
                    threadWaitCreator = pthread_create(&threadWait[qtdThreadsWait - 1], NULL, waitThread, (void *) &taskids[qtdThreadsWait - 1]);     

                    if (threadWaitCreator){  //Garantir que a Thread foi criada corretamente!

                        printf("ERRO CRIACAO DE THREADWAIT! %d\n", threadWaitCreator);         
                        exit(-1);  

                    }

                    qtdThreadsWait++;
                    threadWait = (pthread_t*) calloc(qtdThreadsWait,sizeof(pthread_t));    //Criando vetor que tera as threadsWait
                    taskids = (int *) calloc(qtdThreadsWait, sizeof(int));        
                    modifiquei = 1;

                }
            }
            
        }

        fclose(arquivoEntrada);
    }

    for(i = 0; i < qtdThreadsWait; i++){    //Esperar todas as threadsWait!

        pthread_join(threadWait[i], NULL);

    }

    pthread_exit(NULL);

}

int main (int argc, char *argv[]){ 

    pthread_t *threads;
    int *taskids;
    int threadCreator;
    int i;

    printf("Digite a quantidade de Arquivos a serem lidos:\n");
    scanf("%d", &qtdArquivos);

    if( qtdArquivos < 1){

        printf("ERRO: Quantidade de Arquivos invalida!\n");
        exit(-1);

    }

    printf("Digite a quantidade de Threads que serao usadas:\n");
    scanf("%d", &qtdThreads);

    if( qtdThreads < 1 || qtdThreads > qtdArquivos){

        printf("ERRO: Quantidade de threads invalida!\n");
        exit(-1);

    }

    printf("Digite a quantidade de Linhas da tabela que serao usadas:\n");
    scanf("%d", &qtdLinhas);

    if( qtdLinhas < 1){

        printf("ERRO: Quantidade de linhas invalida!\n");
        exit(-1);

    }

    // ---------------------------- Fim da Leitura dos dados ---------------------------------

    threads = (pthread_t*) malloc(qtdThreads*sizeof(pthread_t));    //Criando vetor que tera as threads
    taskids = (int *) malloc(qtdThreads*sizeof(int));
    myVectorMutex = (pthread_mutex_t *) malloc(qtdLinhas*sizeof(pthread_mutex_t));  //Inicializando vetor de Mutex!

    for(i = 0; i < qtdLinhas; i++){

        pthread_mutex_init(&myVectorMutex[i], NULL);

    }

    tabelaOutput = (char **) malloc(qtdLinhas*sizeof(char *));  //Inicializando a tabela global

    for(i = 0; i < qtdLinhas; i++){

        tabelaOutput[i] = (char *) malloc((QTD_CHAR_LINHA + 1)*sizeof(char));
        strcpy(tabelaOutput[i], "XXXXXX -----------  XX:XX");

    }

    // ---------------------------- Fim da Inicialização dos dados ---------------------------------

    for(i = 0; i < qtdThreads; i++){    //Realizar a criação das threads!  
        
        taskids[i] = i; 
        threadCreator = pthread_create(&threads[i], NULL, changeTable, (void *) &taskids[i]);     

        if (threadCreator){  //Garantir que a Thread foi criada corretamente!

        printf("ERRO; código de retorno é %d\n", threadCreator);         
        exit(-1);  

        }   
    }

    // ---------------------------- Fim da Criação das Threads ---------------------------------

    for(i = 0; i < qtdThreads; i++) {   //Necessario esperar todas as threads finalizarem para colocar o resultado final!

        pthread_join(threads[i], NULL);

    }

    system("clear");    //Debugando!

    for(i = 0; i < qtdLinhas; i++){ 

        printf("%s" "%s" "%s\n", colors[i % QTD_COLORS], tabelaOutput[i], reset);

    }

    printf("Fim da execucao de todas as Threads...\n");
    pthread_exit(NULL);

}

// gcc -pthread questao02.c -o questao02 && ./questao02 //COMPILAR e EXECUTAR!

