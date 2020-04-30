#include <stdbool.h>
#include <stdint.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void init_futhark();
extern void render(bool *pixels, uint32_t width, uint32_t height);
void destroy_futhark();

void main(int argc, const char *argv[])
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

    bool *src = malloc(width * height * sizeof(bool));
    init_futhark();
    
    struct timespec start, finish;
    double seconds_elapsed = 0.0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (uint32_t i = 0; i < iteration; i++)
    {
        render(src, width, height);   
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    seconds_elapsed += (double)(finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1.0e9;
    printf("Result: %lf\n", seconds_elapsed);
    
    destroy_futhark();
    free(src);
}