#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <chrono>
#include <iostream>

#define NUMTHREADS_POW2 2   // 2^x number of threads to be used during || sort 
#define ARRAY_SIZE 10000    // Size of random array

/*
Merge sort code taken from
https://github.com/Shawn314/My-data-struct-and-algorithm/blob/bb817e005c5ec73cde557a1076a7dd7d095a7da9/sort.cpp (via MS Copilot)
*/

void merge(int *array, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int *L = (int*) malloc(n1 * sizeof(int));
    int *R = (int*) malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; ++i) {
        L[i] = array[left + i];
    }
    for (int j = 0; j < n2; ++j) {
        R[j] = array[mid + 1 + j];
    }

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            array[k] = L[i];
            ++i;
        } else {
            array[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        array[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        array[k] = R[j];
        ++j;
        ++k;
    }

    free(L);
    free(R);
}

void merge_sort(int *array, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        merge_sort(array, left, mid);
        merge_sort(array, mid + 1, right);
        merge(array, left, mid, right);
    }
}

// Struct to hold necessary info for a single thread in the ||ed sort
struct MergeSorterThreadArgs {
    int* Elements;
    int maxDepth;
    int lbound;
    int rbound;
};

void* parallelized_merge_sort(void *args)
{
    MergeSorterThreadArgs *mst_args = (MergeSorterThreadArgs *)args;

    int middle_element;

    // If not yet at maxDepth... further parallelize
    bool further_parallelize = mst_args->maxDepth > 0;

    if(mst_args->lbound < mst_args->rbound)
    {
        // Find middle element
        middle_element = mst_args->lbound + (mst_args->rbound - mst_args->lbound) / 2; 

        if(further_parallelize)
        {
            pthread_t l_thrd, r_thrd;

            // Define args for threads that will handle left and right sides respectively
            MergeSorterThreadArgs l_thrd_args = {mst_args->Elements, mst_args->maxDepth-1 , mst_args->lbound, middle_element};
            MergeSorterThreadArgs r_thrd_args = {mst_args->Elements, mst_args->maxDepth-1 , middle_element+1, mst_args->rbound};

            // Create threads with the parallelized_merge_sort as starting func
            pthread_create(&l_thrd, NULL, parallelized_merge_sort, &l_thrd_args);
            pthread_create(&r_thrd, NULL, parallelized_merge_sort, &r_thrd_args);

            // Wait for subarray-sorting-threads to complete and join them back
            pthread_join(l_thrd, NULL);
            pthread_join(r_thrd, NULL); // **NO 2 threads will operate on same part of overall array at the same time bc the 
                                        // higher-level sort will wait for the subarray sorts to continue
        }
        else
        {
            // If not further parallelized, perform lower levels of MS algorithm on the same thread
            merge_sort(mst_args->Elements, mst_args->lbound, middle_element);
            merge_sort(mst_args->Elements, middle_element + 1, mst_args->rbound);
        }

        //Merge the two sorted sides
         merge(mst_args->Elements, mst_args->lbound, middle_element, mst_args->rbound);
    }
    
    return NULL;
}

int main() {

int array[ARRAY_SIZE];

    // Seed the random number generator
    srand(time(0));

    // Populate the array with random numbers between 0 and 100
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        array[i] = rand()%100;
    }
    int n = sizeof(array) / sizeof(array[0]);

    MergeSorterThreadArgs data = {array, NUMTHREADS_POW2, 0, n - 1};
    
    // Get the start time
    auto start = std::chrono::high_resolution_clock::now();

    pthread_t main_thread;
    pthread_create(&main_thread, NULL, parallelized_merge_sort, &data);
    pthread_join(main_thread, NULL);

     // Get the end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> duration = end - start;

    // for(int i=0;i<ARRAY_SIZE; i++)
    // {
    //     std::cout << array[i] << " ";
    // }

    // Output the execution time in seconds
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}