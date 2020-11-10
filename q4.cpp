/*
Projeto de Threads 2020.3
Data: 02/11/20
Grupo:  Allan Soares Vasconcelos <asv>
        Amanda Lima Lassere <all2>
        Macio Monteiro de Meneses Jr <mmmj>
        Maria Isabel Fernandes dos Santos <mifs>

4. Para facilitar e gerenciar os recursos de um sistema computacional com múltiplos processadores (ou núcleos),
você deverá desenvolver uma API para tratar requisições de chamadas de funções em threads diferentes. 
*/


#include <iostream>
#include <pthread.h>
#include <queue>
#include <vector>

using namespace std;

int qtdProcessadores;
int qtdRequisicoes;
int threadsEmUso = 0;

typedef struct funexec{
    int identificador;
    void* (*funcao)(void *);
    void* parametros;
}Funexec;

void** arrayDeResultados;
queue<Funexec> buffer;

pthread_t threadDoDespachante; 

pthread_mutex_t mutexDespachanteBuffer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexDespachanteNumThreads = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t* arrayDeMutexes; 

pthread_cond_t* arrayDeCondicoes;
pthread_cond_t condicaoThreadsEmUso = PTHREAD_COND_INITIALIZER;
pthread_cond_t condicaoThreadsNoBuffer = PTHREAD_COND_INITIALIZER;

pthread_cond_t condicaoGeral = PTHREAD_COND_INITIALIZER;

int id = 0;

int agendarExecucao(void* (*ponteiroPraFunc)(void *), void* ponteiroParametros){ //parametros sao a função a ser executada e a struct com os parametros dela
    
    Funexec entrada; //coloca no buffer a requisição

    entrada.funcao = ponteiroPraFunc;    // Precisamos que func seja do tipo: void* (*funcao)(void *);   
    entrada.parametros = ponteiroParametros;
    entrada.identificador = id;
    id++;

    pthread_mutex_lock(&mutexDespachanteBuffer);

    buffer.push(entrada);
    pthread_cond_signal(&condicaoThreadsNoBuffer);  // Acorda a thread despachante, caso ela esteja dormindo por não ter nenhuma requisição na fila
    pthread_mutex_unlock(&mutexDespachanteBuffer);

    return id-1; // Retorna o id da função colocada no buffer
}

void* pegarResultadoExecucao(int idAtual){ //fazer um mutex para cada thread para acordar ela quando necessario
    
    pthread_mutex_lock(&arrayDeMutexes[idAtual]); // Espera pelo fim da execução da função caso não tenha terminado
    while(arrayDeResultados[idAtual] == NULL){
        pthread_cond_wait(&arrayDeCondicoes[idAtual], &arrayDeMutexes[idAtual]);
    }

    pthread_mutex_destroy(&arrayDeMutexes[idAtual]);
    pthread_cond_destroy(&arrayDeCondicoes[idAtual]);

    return arrayDeResultados[idAtual];
}

void* executaFuncao(void* ponteiroFuncStrct){

    Funexec funcStrct = *((Funexec *) ponteiroFuncStrct);

    int idAtual = funcStrct.identificador;

    void* resultado = (void *) malloc(sizeof(void *));
    resultado = funcStrct.funcao(funcStrct.parametros);


    pthread_mutex_lock(&arrayDeMutexes[idAtual]);

    arrayDeResultados[idAtual] = resultado;
    pthread_cond_signal(&arrayDeCondicoes[idAtual]);     // Acorda a função pegarResultadoExecucao para ela saber que o resultado já está pronto 
    pthread_mutex_unlock(&arrayDeMutexes[idAtual]);

    pthread_mutex_lock(&mutexDespachanteNumThreads);    // Esse lock no mutex também é necessário pois threadsEmUso é um recurso compartilhado no qual pode ocorrer uma condição de corrida
    
    threadsEmUso--;
    pthread_cond_broadcast(&condicaoThreadsEmUso);     // Acorda a thread despachante caso ela esteja dormindo se threadsEmUso > N

    pthread_mutex_unlock(&mutexDespachanteNumThreads);

    pthread_exit(NULL);
}

void* threadDespachante(void* nada){ 
  
    for(int req = 0; req <qtdRequisicoes; req++){

        pthread_mutex_lock(&mutexDespachanteBuffer);
        pthread_mutex_lock(&mutexDespachanteNumThreads);
        while(buffer.empty() || threadsEmUso >= qtdProcessadores){
            
            if(buffer.empty()){
                pthread_cond_wait(&condicaoThreadsNoBuffer, &mutexDespachanteBuffer);
            }

            if(threadsEmUso >= qtdProcessadores){
                pthread_cond_wait(&condicaoThreadsEmUso, &mutexDespachanteNumThreads);
            }

        }
        pthread_mutex_unlock(&mutexDespachanteNumThreads);
        pthread_mutex_unlock(&mutexDespachanteBuffer);

        Funexec *strctDaFuncao = (Funexec *) malloc(sizeof(Funexec));
        *strctDaFuncao = buffer.front(); 
        buffer.pop(); 

        pthread_t *threadAtual = (pthread_t *) malloc(sizeof(pthread_t));
        pthread_create(threadAtual, NULL, executaFuncao, (void *) strctDaFuncao);
        threadsEmUso++;
    }

    pthread_exit(NULL);
}

void iniciarListaEArrayDeMutexes(){

    arrayDeMutexes = (pthread_mutex_t *) malloc(qtdRequisicoes*sizeof(pthread_mutex_t));
    arrayDeCondicoes = (pthread_cond_t *) malloc(qtdRequisicoes*sizeof(pthread_cond_t));
    arrayDeResultados = (void **) malloc(qtdRequisicoes*sizeof(void *));
    
    for(int i=0; i<qtdRequisicoes; i++){
        
        pthread_mutex_init(&arrayDeMutexes[i], NULL); //Inicaliza cada mutex do arrayDeMutexes e cada condicao do arrayDeCondicoes
        pthread_cond_init(&arrayDeCondicoes[i], NULL);

        void* aux = (void *) malloc(sizeof(void *));
        arrayDeResultados[i] = aux;
        
        arrayDeResultados[i] = NULL;
    }
}

void inicializar(){

    cout << "Digite a quantidade de processadores desejada: " << endl;
    cin >> qtdProcessadores;
    cout << "Digite a quantidade de requisições desejada: " << endl;
    cin >> qtdRequisicoes;

    iniciarListaEArrayDeMutexes();

    if(pthread_create(&threadDoDespachante, NULL, threadDespachante, NULL)){
        printf("Erro ao criar a thread despachante\n");
        exit(-1);
    }

}

void finalizar(){
    for(int i=0; i < qtdRequisicoes; i++){
        free(arrayDeResultados[i]);
    }
    free(arrayDeMutexes);
    free(arrayDeCondicoes);
}

typedef struct pardeint{
    int a;
    int b;
}parInt;

void* somaFeia(void* parametro){
    parInt p = (*(parInt *) parametro);
    int* ponteiroPraSoma = (int *) malloc(sizeof(int));

    for(int i=0; i<1000; i++){}

    *ponteiroPraSoma = p.a + p.b;
    return (void *) ponteiroPraSoma;
}

int main() {

    inicializar();
    parInt** vetorPonteiroArgumentos = (parInt **) malloc(qtdRequisicoes*sizeof(parInt *));
    int idDeCadaRequisicao[qtdRequisicoes] = {0};
    
    for(int i=0; i<qtdRequisicoes; i++){
        vetorPonteiroArgumentos[i] = (parInt *) malloc(sizeof(parInt));

        vetorPonteiroArgumentos[i]->a = i;
        vetorPonteiroArgumentos[i]->b = i;
    }

    for(int i=0;i<qtdRequisicoes;i++){
        
        idDeCadaRequisicao[i] = agendarExecucao(somaFeia , vetorPonteiroArgumentos[i]);  
    }

    for(int i=0;i<qtdRequisicoes;i++){

        int result = *((int *) pegarResultadoExecucao(idDeCadaRequisicao[i]));
        cout << "Resultado " << i << ": " << result << endl;
    }

    finalizar();
    
    return 0;
}

// g++ -pthread questao04.cpp -o questao06 && ./questao06 //COMPILAR e EXECUTAR!