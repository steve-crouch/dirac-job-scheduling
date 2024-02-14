#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int num_threads, t;
    int *results;

    // Use an OpenMP library function to get the number of threads
    num_threads = omp_get_max_threads();

    // Create a buffer to hold the integer results from each thread
    results = malloc(sizeof(*results) * num_threads);

    // In parallel, within each thread available, store the thread's
    // number in our shared results buffer
    #pragma omp parallel shared(results)
    {
        int my_thread = omp_get_thread_num();
        results[my_thread] = my_thread;
    }

    for (t = 0; t < num_threads; t++)
    {
        printf("Hello world thread number received from thread %d\n", t);
    }
}
