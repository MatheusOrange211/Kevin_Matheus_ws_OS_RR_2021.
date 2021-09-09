#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define N 5                 /* numero de filosofos*/
#define THINKING 0          /* o filosofo esta pensando */
#define HUNGRY 1            /* o filosofo esta tentando peger garfos*/
#define EATING 2            /* o filosofo esta comendo */

int state[N];              /* arranjo para controlar o estado de cada filosofo */
pthread_t philosophersThreads[N];
pthread_mutex_t mutexGeneral = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexPhilosophers[N];

int LEFT(int i) {             /* numero do vizinho a esquerda de i */
  return (i+N-1)%N;
}

int RIGHT(int i) {            /* numero do vizinho a direita de i */
  return (i+1)%N;
}

int test(int i) {             /*i: o numero do filosofo, de 0 a N-1 */
  if (state[i] == HUNGRY && 
      state[LEFT(i)] != EATING &&
      state[RIGHT(i)] != EATING) {
        state[i] = EATING;
        pthread_mutex_unlock(&mutexPhilosophers[i]);

        printf("O filosofo %d pode comer\n", i);
        return 1;
  }
  return 0;
}

void think(int i) {             /* tempo que o filosofo esta pensando */
  printf("%d - Pensando\n", i);
  sleep(rand() % 4);  // thinking for 5 sec
}

void eat(int i) {               /* tempo que ele come */
  printf("%d - Comendo\n", i);
  sleep(rand() % 4);  // eating for 5 sec
}

void take_forks(int i) {                     /* i: o numero do filosofo, de 0 a N-1 */ 
  pthread_mutex_lock(&mutexGeneral);         /* entra na regiao critica */
  state[i] = HUNGRY;                         /* registra que o filosofo esta faminto */
  test(i);                                   /* tenta pegar dois garfos */
  pthread_mutex_unlock(&mutexGeneral);       /* sai da regiao critica */
  pthread_mutex_lock(&mutexPhilosophers[i]); /* bloqueia se os garfos nao foram pegos */
}

void put_forks(int i) {                      /* i: o numero do filosofo, de 0 a N-1 */
  int leftPhilosopher, rightPhilosopher;     
  pthread_mutex_lock(&mutexGeneral);         /* entra na regiao critica */
  state[i] = THINKING;                       /* o filosofo acabou de comer */
  rightPhilosopher = test(RIGHT(i));         /* ve se o vizinho da esquerda pode comer agora */
  leftPhilosopher = test(LEFT(i));           /* ve se o vizinho da direita pode comer agora */

  if (rightPhilosopher) {
    printf("O filosofo %d, deu seu garfo para o filosofo %d comer\n", i, RIGHT(i));
  }

  if (leftPhilosopher) {
    printf("O filosofo %d, deu seu garfo para o filosofo %d comer\n", i, LEFT(i));
  }

  pthread_mutex_unlock(&mutexGeneral);        /* sai da regiao critica */
}

void* philosopher(void* i) {                  /* i: o numero do filosofo, de 0 a N-1*/
  int philosopherNumber = atoi( (char*) i ); // transform array of char in int to identify a philosopher
  
  printf("Inicializando filosofo número %d\n", philosopherNumber);

  while (1) {             					  /* repete para sempre */
    think(philosopherNumber);                 /* o filosofo esta pensando */
    take_forks(philosopherNumber);            /* pega dois garfos ou bloqueia */
    eat(philosopherNumber);                   /* hummm, espaguete! */
    put_forks(philosopherNumber);             /* devolve os dois garfos a mesa */
  }

  pthread_exit((void *)0);                    
}

int main() {                  
  char numberInString[4][32];                // array para obter o número de cada filósofo
  srand(0);                                   // para obter o mesmo resultado nos números aleatórios para testar
  
  printf("--- Inicializando semaforos dos filosofos ---\n");
  for (int i = 0; i < N; i++) {
    pthread_mutex_init(&mutexPhilosophers[i], NULL);
  }

  printf("--- Inicializando threads dos filosofos ---\n");
  for (int i = 0; i < N; i++) {
    sprintf(numberInString[i], "%d", i);

    pthread_create(&philosophersThreads[i], NULL, philosopher, (void*) numberInString[i]);
  }

  for (int i = 0; i < N; i++) {
    pthread_join(philosophersThreads[i], NULL); //eh onde ocorre a sincronizacao
  }

  pthread_exit(NULL);
}


