#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "../backend.h"

int main(int argc, const char *argv[])
{
    uint32_t iteration, max_value;
    int32_t width, length;
    bool parity;

    if (argc < 5 || argc > 6)
    {
        printf("Usage:\n    %s iteration width height automaton [max_value]\n\
            automaton: parity or cyclic\n\
            max_value: uint, mandatory for cyclic automation, ignored for parity\n", argv[0]);
        exit(1);
    }
    else
    {
        iteration = atoi(argv[1]);
        width = atoi(argv[2]);
        length = width * atoi(argv[3]);

        if (!strcmp(argv[4], "parity"))
        {
            parity = true;
            max_value = 1;
        }
        else if (!strcmp(argv[4], "cyclic") && argc == 6)
        {
            parity = false;
            max_value = atoi(argv[5]);
        }
        else
        {
            printf("Automaton not implemented\n");
            exit(1);
        }
    }

    srand(0);
    uint32_t *src = malloc(length * sizeof(uint32_t));
    
    for (int32_t i = 0; i < length; i++)
    {
        src[i] = rand() % (max_value+1);
    }

    init();
    struct timespec start, finish;
    double seconds_elapsed = 0.0;

    set_args(parity, src, width, length, 1);
    clock_gettime(CLOCK_MONOTONIC, &start);
    iterate(iteration);
    get_result(src);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    seconds_elapsed += (double)(finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1.0e9;
    printf("Result: %lf\n", seconds_elapsed);

    destroy();
    free(src);
}