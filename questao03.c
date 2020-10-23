/*

Projeto de Threads 2020.3
Data: 22/10/20
Grupo:  Allan Soares Vasconcelos <asv>
        Amanda Lima Lassere <all2>
        Macio Monteiro de Meneses Jr <mmmj>
        Maria Isabel Fernandes dos Santos <mifs>

3. Considere 2 strings s1 and s2. Crie um programa em C usando pthreads 
para encontrar a quantidade de substrings em s1 que são iguais a s2. Por exemplo, 
suponha que quantidade_substring(s1, s2) implementa a funcionalidade, então quantidade_substring(“abcdab”, “ab”) = 2, 
quantidade_substring(“aaa”, “a”) = 3, quantidade_substring(“abac”, “bc”) = 0. 
Assuma o tamanho de s1 e s2 seja n1 e n2, respectivamente, and p threads são adotadas. 
Assuma também que n1 mod p = 0 (resto da divisão), e n2 < n1/p . 
Adicionalmente, considere que a string s1 é uniformemente particionada para que p threads  pesquisem concorrentemente 
por substrings. Depois que uma thread termina a busca e obtenha um quantidade de substrings, este contador deverá ser 
incrementado a uma variável global que terá o total de todas substrings s2 em s1. Quando todas as threads terminarem, 
o resultado deverá ser exibido.

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR_STRING 100 //Modificar aqui para suportar strings maiores ou menores.

char string1[MAX_CHAR_STRING];
char string2[MAX_CHAR_STRING];
char **subStrings;
int qtdThreads, tamString1, tamString2;
int totalSubStrings = 0;    //Variavel Global que sera incrementada!
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;  //Mutex que sera feito para impedir a condição de corrida!

void* quantidade_substring(void * threadid){  //Função que será realizada pelas Threads 

    int tid = *((int *)threadid); 
    char *stringParticionada = (char *) malloc(( (tamString1/qtdThreads) + 1)*sizeof(char));
    strcpy(stringParticionada, subStrings[tid]);   

    printf("Thread #%d inicializada com subString [%s]...\n", tid, stringParticionada); //Debugando!

    int resultadoLocal = 0;
    char* str = stringParticionada;

    while ( (str = strstr(str, string2)) != NULL ){ //Contando quantidade de substring
        resultadoLocal++;
        str++;
    }

    printf("Thread #%d contou [%d] subStrings\n", tid, resultadoLocal);
    int contei = 0;

    while(contei == 0){ //Realizando a soma na variavel global!

        if ( pthread_mutex_trylock(&myMutex) == 0){
            
            totalSubStrings += resultadoLocal;
            pthread_mutex_unlock(&myMutex);
            contei = 1;
        }
    }

    pthread_exit(NULL);
}

int main (int argc, char *argv[]){   

    pthread_t *threads;
    int *taskids;
    int threadCreator, i, k, j;

    printf("Digite a primeira String:\n");
    scanf("%s",string1);
    printf("Digite a segunda String:\n");
    scanf("%s",string2);
    printf("Digite a quantidade de Threads a serem utilizadas:\n");
    scanf("%d", &qtdThreads);

    //Estamos assumindo que: tamString1 mod p = 0 e (tamString2 < tamString1/p) . 

    threads = (pthread_t*) malloc(qtdThreads*sizeof(pthread_t));    //Criando vetor que tera as threads
    tamString1 = strlen(string1);
    tamString2 = strlen(string2);
    subStrings = (char **) malloc(qtdThreads*sizeof(char *));  //Alocando espaço das substrings
    taskids = (int *) malloc(qtdThreads*sizeof(int));

    for(i = 0; i < qtdThreads; i++){ //Inicializando espaço das substrings

        subStrings[i] = (char *) malloc(( (tamString1/qtdThreads) + 1)*sizeof(char));

    }

    for(i = 0, k = 0, j = 0; i < tamString1; i++, j++){    //Responsavel por particionar as substrings!

        subStrings[k][j] = string1[i]; 

        if(j == (tamString1/qtdThreads) - 1){
            
            subStrings[k][j+1] = '\0';
            j = -1;
            k++; 
        }
    }

    for(i = 0; i < qtdThreads; i++){    //Debugando!

        printf("Substring [%d] = [%s]\n", i, subStrings[i]);

    }

    for(i = 0; i < qtdThreads; i++){    //Realizar a criação das threads!  
        
        taskids[i] = i;
	    printf("Main: criando thread #%d\n", i);   //Somente Debugando!   
        threadCreator = pthread_create(&threads[i], NULL, quantidade_substring, (void *) &taskids[i]);     

        if (threadCreator){  //Garantir que a Thread foi criada corretamente!

        printf("ERRO; código de retorno é %d\n", threadCreator);         
        exit(-1);  

        }   
    }

    for(i = 0; i < qtdThreads; i++) {   //Necessario esperar todas as threads finalizarem para colocar o resultado final!

        pthread_join(threads[i], NULL);
        printf("Thread #%d finalizada...\n", i);

    }

    printf("Total de Substring [%s] em [%s] eh: [%d]\n", string2, string1,totalSubStrings);
    pthread_exit(NULL);
}

// gcc -pthread questao03.c -o questao03 && ./questao03 //COMPILAR e EXECUTAR!