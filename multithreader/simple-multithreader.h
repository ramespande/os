#include <iostream>
#include <list>
#include <functional>
#include <stdlib.h>
#include <cstring>
#include <time.h>

int user_main(int argc, char **argv);

/* Demonstration on how to pass lambda as parameter.
 * "&&" means r-value reference. You may read about it online.
 */
void demonstration(std::function<void()> && lambda) {
  lambda();
}

int main(int argc, char **argv) {
  /*
   * Declaration of a sample C++ lambda function
   * that captures variable 'x' by value and 'y'
   * by reference. Global variables are by default
   * captured by reference and are not to be supplied
   * in the capture list. Only local variables must be
   * explicity captured if they are used inside lambda.
   */
  int x=5,y=1;
  // Declaring a lambda expression that accepts void type parameter
  auto /*name*/ lambda1 = /*capture list*/[/*by value*/ x, /*by reference*/ &y](void) {
    /* Any changes to 'x' will throw compilation error as x is captured by value */
    y = 5;
    std::cout<<"====== Welcome to Assignment-"<<y<<" of the CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  // Executing the lambda function
  demonstration(lambda1); // the value of x is still 5, but the value of y is now 5

  int rc = user_main(argc, argv);

  auto /*name*/ lambda2 = [/*nothing captured*/]() {
    std::cout<<"====== Hope you enjoyed CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  demonstration(lambda2);
  return rc;
}

#define main user_main

typedef struct{
   int low;
   int high;
   std::function<void(int)> lambda;
} thread_args;

typedef struct {
    int low1, high1;
    int low2, high2;
    std::function<void(int,int)> lambda;
} thread_args_2d;

void *thread_func(void *ptr){
     thread_args *t = ((thread_args *)ptr);
     for (int i=t->low; i<t->high; i++) t->lambda(i);
     return NULL;
}

void* thread_func_2d(void* ptr) {
    thread_args_2d* t = (thread_args_2d*)ptr;
    for (int i = t->low1; i < t->high1; i++) {
        for (int j = t->low2; j < t->high2; j++) {
            t->lambda(i, j);
        }
    }
    return NULL;
}

void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads){
     if (numThreads < 1) {
    	std::cerr << "Error: numThreads must be >= 1\n";
    	return;
     }
     if (low >= high) {
        std::cerr << "Error: Invalid loop bounds\n";
    	return;
     }
     int size = high-low;
     struct timespec start, end;
     clock_gettime(CLOCK_MONOTONIC, &start);
     pthread_t tid[numThreads];
     thread_args args[numThreads];
     int chunksize = (size+(numThreads-1))/numThreads; //ceil division
     args[0].low = low;
     args[0].high = std::min(high,low+chunksize);
     args[0].lambda = lambda;
     thread_func(&args[0]);
     for (int i=1;i<numThreads;i++){ //main thread will handle the first chunk
          args[i].low = low+i*chunksize;
          args[i].high = std::min(high,low+(i+1)*chunksize);
          args[i].lambda = lambda;
          pthread_create(&tid[i],NULL,thread_func,(void*) &args[i]);
     }
     for (int i=1;i<numThreads;i++){
          pthread_join(tid[i],NULL);
     }
     clock_gettime(CLOCK_MONOTONIC, &end);
     double time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
     printf("Time: %f seconds\n", time);
}

void parallel_for(int low1, int high1,int low2, int high2,std::function<void(int,int)> &&lambda,int numThreads) {
    if (numThreads < 1) {
    	std::cerr << "Error: numThreads must be >= 1\n";
    	return;
    }
    if (low1 >= high1) {
    	std::cerr << "Error: Invalid outer loop bounds\n";
    	return;
    }
    int size = high1 - low1;
    int chunk = (size + (numThreads - 1)) / numThreads;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t tid[numThreads];
    thread_args_2d args[numThreads];
    args[0].low1 = low1;
    args[0].high1 = std::min(high1, low1 + chunk);
    args[0].low2 = low2;
    args[0].high2 = high2;
    args[0].lambda = lambda;
    thread_func_2d(&args[0]);
    //main thread will do chunk 0
    for (int t = 1; t < numThreads; t++) {
        args[t].low1 = low1 + t * chunk;
        args[t].high1 = std::min(high1, low1 + (t+1) * chunk);
        args[t].low2 = low2;
        args[t].high2 = high2;
        args[t].lambda = lambda;
        pthread_create(&tid[t], NULL, thread_func_2d, (void*)&args[t]);
    }
    for (int t = 1; t < numThreads; t++) {
        pthread_join(tid[t], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time: %f seconds\n", time);
}
