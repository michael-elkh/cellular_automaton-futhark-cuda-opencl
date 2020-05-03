#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../backend.h"

int main(int argc, const char *argv[])
{
    uint32_t iteration = 1000, width = 1000, height = 1000;
    if (argc > 1)
    {
        iteration = atoi(argv[1]);
    }else{
        printf("Usage:\n    %s iteration [width] [height]\n", argv[0]);
        exit(1);
    }
    
    if(argc > 2){
        width = atoi(argv[2]);
        height = width;
    }
    else if (argc > 3)
    {
        width = atoi(argv[2]);
        height = atoi(argv[3]);
    }

    uint32_t *src = malloc(width * height * sizeof(uint32_t));
    init();
    
    struct timespec start, finish;
    double seconds_elapsed = 0.0;

    set_args(true, src, width, width*height, 1);
    clock_gettime(CLOCK_MONOTONIC, &start);
    iterate(iteration);
    get_result(src);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    seconds_elapsed += (double)(finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1.0e9;
    printf("Result: %lf\n", seconds_elapsed);

    destroy();
    free(src);
}